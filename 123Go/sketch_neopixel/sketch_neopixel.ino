#include <Adafruit_NeoPixel.h>

#define PIN 12        // Pin where NeoPixel is connected
#define NUM_PIXELS 64 // Number of LEDs in the strip
#define BUTTON_A_PIN 14
#define BUTTON_B_PIN 27

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_PIXELS, PIN, NEO_GRB + NEO_KHZ800);

bool BWasPressed = false;
uint32_t redColor = strip.Color(50, 0, 0);
uint32_t greenColor = strip.Color(0, 50, 0);
uint32_t yellowColor = strip.Color(50, 50, 0);
uint32_t blackColor = strip.Color(0, 0, 0);

void showRange(uint16_t start, uint16_t length, uint32_t color) {
    for (uint16_t i = start; i < start + length; i++) {
        strip.setPixelColor(i, color);
    }
    strip.show();
}

void initialise() {
    strip.clear();
    strip.setBrightness(50);
    strip.show();
}

void go() {
    strip.fill(blackColor, 0, NUM_PIXELS);
    strip.show();

    uint16_t ranges[8] = {0, 8, 16, 24, 32, 40, 48, 56};
    for (uint8_t i = 0; i < 8; i += 2) {
        showRange(ranges[i], 8, redColor);
        showRange(ranges[i+1], 8, redColor);
        delay(1000);
    }

    strip.fill(greenColor, 0, NUM_PIXELS);
    strip.show();
    delay(2000);

    strip.fill(blackColor, 0, NUM_PIXELS);
    strip.show();
}

void setup() {
    strip.begin();
    initialise();

     pinMode(BUTTON_A_PIN, INPUT_PULLUP);
     pinMode(BUTTON_B_PIN, INPUT_PULLUP);
}

void loop() {

    if (digitalRead(BUTTON_A_PIN) == LOW) { // Replace BUTTON_A_PIN with the pin connected to Button A
        go();
    }

    if (digitalRead(BUTTON_B_PIN) == LOW) { // Replace BUTTON_B_PIN with the pin connected to Button B
        BWasPressed = true;
        strip.fill(yellowColor, 0, NUM_PIXELS);
        strip.show();
        delay(100);
        strip.fill(blackColor, 0, NUM_PIXELS);
        strip.show();
        delay(100);
    } else if (BWasPressed) {
        BWasPressed = false;
        strip.fill(redColor, 0, NUM_PIXELS);
        strip.show();
        delay(1000);
    }
}

