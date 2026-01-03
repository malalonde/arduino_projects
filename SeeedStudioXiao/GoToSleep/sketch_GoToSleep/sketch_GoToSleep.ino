#include <Adafruit_TinyUSB.h>


#define LED_PIN LED_BUILTIN //Red
#define BUTTON_PIN D10

void setup() {

    digitalWrite(LED_BLUE, LOW); 
    //Wait 10 seconds just in case
    delay(10000);

    // 1. Initialize the LED pin as an output.
    pinMode(LED_PIN, OUTPUT);
    // 2. Turn the LED ON.
    digitalWrite(LED_PIN, LOW); 

    // Initialise pin for Button
    pinMode(BUTTON_PIN, INPUT_PULLUP);
    
    // Optional: Include Serial setup to confirm basic functionality is restored
    Serial.begin(115200);
    //Wait for the USB port to open (essential for native USB boards)
    while (!Serial) { 
        delay(1); 
    }
    Serial.println("Hello! LED is now ON.");
}

void goToSleep(int wakeUpButtonPin)
{
    Serial.println("Going to sleep.");
    // Reconfigure the button pin specifically for wake-up from System OFF
    // Use INPUT_PULLUP and SENSE_LOW to detect a button press (connects pin to GND)
    nrf_gpio_cfg_sense_input(g_ADigitalPinMap[wakeUpButtonPin], NRF_GPIO_PIN_PULLUP, NRF_GPIO_PIN_SENSE_LOW);
    NRF_POWER->SYSTEMOFF = 1;
}

void loop() {
    // Test the button and light the LED if pressed
    if(digitalRead(BUTTON_PIN) == LOW)
    {
        
        Serial.println("Button Pressed.");
        digitalWrite(LED_GREEN, LOW); 

        delay(2000);

        // Turn off all the LEDs
        digitalWrite(LED_GREEN, HIGH); 
        digitalWrite(LED_BLUE, HIGH); 
        digitalWrite(LED_PIN, HIGH); 

        goToSleep(BUTTON_PIN);
    }

    // Go to sleep after 60 seconds
    if(millis() > 60000)
    {
        Serial.println("Time expired, going to sleep.");
        
        // Turn off all the LEDs
        digitalWrite(LED_GREEN, HIGH); 
        digitalWrite(LED_BLUE, HIGH); 
        digitalWrite(LED_PIN, LOW); 

        goToSleep(BUTTON_PIN);
    }

    delay(100);
}