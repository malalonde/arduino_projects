#include <Adafruit_TinyUSB.h>

#define LED_PIN LED_BUILTIN 

void setup() {
    // 1. Initialize the LED pin as an output.
    pinMode(LED_PIN, OUTPUT);
    
    // 2. Turn the LED ON.
    digitalWrite(LED_PIN, LOW); 
    
    // Optional: Include Serial setup to confirm basic functionality is restored
    Serial.begin(115200);
    //Wait for the USB port to open (essential for native USB boards)
    while (!Serial) { 
        delay(1); 
    }
    Serial.println("Hello! LED is now ON.");
}

void loop() {
    // This sketch does nothing in the loop, consuming minimal power 
    // while keeping the CPU fully active.
}