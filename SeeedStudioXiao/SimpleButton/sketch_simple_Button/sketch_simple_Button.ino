#include <Adafruit_TinyUSB.h>

#define LED_PIN LED_BUILTIN 
#define BUTTON_PIN D7

void setup() {
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

void loop() {
    // Test the button and light the LED if pressed
    if(digitalRead(BUTTON_PIN) == LOW)
    {
        digitalWrite(LED_PIN, LOW); 
    }
    else
    {
        digitalWrite(LED_PIN, HIGH); 
    }
    delay(100);
}