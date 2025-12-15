#include <Arduino.h>
#include <Adafruit_TinyUSB.h>

// Define the pin for the built-in LED (often PIN_LED1 or PIN_LED_BLUE)
// Check your board's documentation for the correct pin name/number.
#define LED_PIN         LED_BUILTIN 

// Define the pin for the button (D7)
const int WAKE_UP_PIN = 7; 

// Define a value to store in GPREGRET to indicate the target state
const uint32_t STATE_SLEEP = 0x55AA; // Unique value to signify "Go to Sleep"
const uint32_t STATE_AWAKE = 0xAA55; // Unique value to signify "Stay Awake"

// --- Function to put the device into Deep Sleep (System OFF) ---
// Note: This function will halt execution until a wake-up event (button press).
void enterDeepSleep() {
    Serial.println("LED OFF. Entering System OFF...");
    digitalWrite(LED_PIN, LOW);
    delay(100); // Wait for serial output to flush

    // 1. Configure the wake-up source (D7)
    // We attach the interrupt to trigger on a FALLING edge (standard pull-up button press).
    attachInterrupt(digitalPinToInterrupt(WAKE_UP_PIN), NULL, FALLING);
    
    // 2. Set the target state for the *next* reboot (which will be AWAKE)
    NRF_POWER->GPREGRET = STATE_AWAKE;

    // 3. Enter System OFF
    // This is the register-level command for the nRF52840
    NRF_POWER->SYSTEMOFF = 1;
    
    // The device will restart at setup() after the button is pressed.
}

// --- Function to prepare for a clean reboot and stay Awake ---
void prepareToStayAwake() {
    Serial.println("LED ON. Staying Awake.");
    digitalWrite(LED_PIN, HIGH);
    
    // Set the target state for the *next* time the button is pressed (which will be SLEEP)
    NRF_POWER->GPREGRET = STATE_SLEEP;

    // Detach the interrupt so further presses don't immediately cause a power cycle.
    detachInterrupt(digitalPinToInterrupt(WAKE_UP_PIN));
}

// --- Button Interrupt Service Routine (ISR) ---
void buttonPressISR() {
    // This ISR is used ONLY when the device is AWAKE.
    // It will be triggered by a button press and immediately reset the device
    // to put the new state into effect (SLEEP or AWAKE).
    // The GPREGRET register is already set in the setup() logic below.
    // We use the NVIC_SystemReset() to quickly reboot the system.
    NVIC_SystemReset();
}

void setup() {
    //Add a delay to allow reprogramming in case of mistake
    delay(10000);

    
    // 1. Initialize Serial and LED
    Serial.begin(115200);
    // Note: No need for `while (!Serial)` as we want the code to run quickly on reboot
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);
    
    pinMode(WAKE_UP_PIN, INPUT_PULLUP);
    
    uint32_t previous_state = NRF_POWER->GPREGRET;
    
    // 2. Clear the GPREGRET for the *next* press
    NRF_POWER->GPREGRET = 0; // Clear register after reading its contents

    Serial.print("\n--- Device Rebooted ---\n");
    Serial.print("Previous State (GPREGRET): ");
    Serial.println(previous_state, HEX);
    
    // 3. State Machine Logic
    if (previous_state == STATE_SLEEP) {
        // The device rebooted, and GPREGRET told us to go to sleep.
        enterDeepSleep();
    } else {
        // The device rebooted from System OFF, or it's the first power-up.
        // We configure it to stay awake and respond to a press by going to sleep.
        prepareToStayAwake();
        
        // Now, attach the interrupt to the buttonPressISR to capture the next press
        // which will trigger a reset, making the device go to sleep.
        attachInterrupt(digitalPinToInterrupt(WAKE_UP_PIN), buttonPressISR, FALLING);
    }
}

void loop() {
    // This code only runs when the device is in the AWAKE state.
    Serial.print(".");
    delay(1000);
}