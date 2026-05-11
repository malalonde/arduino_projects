// 123 Go! - Countdown and LED Animation for Adafruit PropMaker
// Converted from CircuitPython to Arduino

#include <Adafruit_NeoPixel.h>

// PropMaker Feather RP2040 pins
#define NEOPIXEL_ENABLE_PIN 10  // PropMaker NeoPixel power enable (GPIO 10)
#define MATRIX_PIN          21  // 8x8 NeoPixel matrix on PropMaker output (GPIO 21)
#define STRIP_PIN           A1  // LED strip on secondary pin (GPIO 27)

// Button pins
#define BUTTON_A_PIN        19  // GPIO19
#define BUTTON_B_PIN        6   // GPIO 8 (D6) - adjust if using second button

// LED counts
#define MATRIX_NUM_LEDS     64
#define STRIP_NUM_LEDS      150
#define LED_DISTANCE        5   // Distance between lit LEDs in strip animation

// Create NeoPixel objects
Adafruit_NeoPixel ledMatrix = Adafruit_NeoPixel(MATRIX_NUM_LEDS, MATRIX_PIN);
Adafruit_NeoPixel strip = Adafruit_NeoPixel(STRIP_NUM_LEDS, STRIP_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel onboardLed = Adafruit_NeoPixel(1, PIN_NEOPIXEL, NEO_GRB + NEO_KHZ800);

// Colors
uint32_t redColor;
uint32_t greenColor;
uint32_t yellowColor;
uint32_t blackColor;
uint32_t blueColor;

// State variables
bool BWasPressed = false;
bool buttonAWasPressed = false;

// Animation state
unsigned long animationStart = 0;
unsigned long animationDuration = 10000;  // milliseconds
bool animationMoving = true;
unsigned long lastAnimationTime = 0;
uint8_t ledIndex = 0;
uint32_t ledColor;
bool ledEnabled = true;

void setup() {
    Serial.begin(115200);
    delay(1000);  // Wait for serial connection
    Serial.println("123 Go! - PropMaker RP2040 Starting...");
    Serial.print("Button A on GPIO");
    Serial.println(BUTTON_A_PIN);
    Serial.print("Button B on GPIO");
    Serial.println(BUTTON_B_PIN);

    // Enable NeoPixel power on PropMaker
    pinMode(NEOPIXEL_ENABLE_PIN, OUTPUT);
    digitalWrite(NEOPIXEL_ENABLE_PIN, HIGH);

    // Enable NeoPixel power on PropMaker
    pinMode(PIN_EXTERNAL_POWER, OUTPUT);
    digitalWrite(PIN_EXTERNAL_POWER, HIGH);

    // Setup buttons with internal pull-ups
    pinMode(BUTTON_A_PIN, INPUT_PULLUP);
    pinMode(BUTTON_B_PIN, INPUT_PULLUP);

    // Initialize NeoPixels
    ledMatrix.begin();
    ledMatrix.setBrightness(128);  // ~25% brightness
    ledMatrix.clear();
    ledMatrix.show();

    strip.begin();
    strip.setBrightness(25);  // ~10% brightness
    strip.clear();
    strip.show();

    onboardLed.begin();
    onboardLed.setBrightness(50);
    onboardLed.clear();
    onboardLed.show();

    // Define colors (after NeoPixel objects are created)
    redColor = ledMatrix.Color(50, 0, 0);
    greenColor = ledMatrix.Color(0, 50, 0);
    yellowColor = ledMatrix.Color(50, 50, 0);
    blackColor = ledMatrix.Color(0, 0, 0);
    blueColor = ledMatrix.Color(0, 0, 50);

    // Smooth blue fade in/out on matrix and onboard LED to indicate ready
    // Fade in
    for (int brightness = 0; brightness <= 50; brightness += 2) {
        uint32_t fadeBlue = ledMatrix.Color(0, 0, brightness);
        ledMatrix.fill(fadeBlue, 0, MATRIX_NUM_LEDS);
        ledMatrix.show();
        onboardLed.setPixelColor(0, fadeBlue);
        onboardLed.show();
        delay(20);
    }
    // Hold
    delay(300);
    // Fade out
    for (int brightness = 50; brightness >= 0; brightness -= 2) {
        uint32_t fadeBlue = ledMatrix.Color(0, 0, brightness);
        ledMatrix.fill(fadeBlue, 0, MATRIX_NUM_LEDS);
        ledMatrix.show();
        onboardLed.setPixelColor(0, fadeBlue);
        onboardLed.show();
        delay(20);
    }
    ledMatrix.clear();
    ledMatrix.show();
    onboardLed.clear();
    onboardLed.show();

    // Initial animation
    ledColor = blueColor;
    startAnimation(blueColor, 10000, true);

    Serial.println("Ready! Waiting for button press...");
}

void showLedStrip(uint8_t index, uint32_t color) {
    if (color != 0xFFFFFFFF) {  // Use 0xFFFFFFFF as "no change" marker
        ledColor = color;
    }
    strip.clear();
    for (int i = index; i < STRIP_NUM_LEDS; i += LED_DISTANCE) {
        strip.setPixelColor(i, ledColor);
    }
    strip.show();
}

void showLedStripCurrentColor(uint8_t index) {
    strip.clear();
    for (int i = index; i < STRIP_NUM_LEDS; i += LED_DISTANCE) {
        strip.setPixelColor(i, ledColor);
    }
    strip.show();
}

void showRange(uint16_t start, uint16_t length, uint32_t color) {
    for (uint16_t i = start; i < start + length; i++) {
        ledMatrix.setPixelColor(i, color);
    }
    ledMatrix.show();
}

void startAnimation(uint32_t iColor, unsigned long duration, bool moving) {
    animationMoving = moving;
    animationDuration = duration;
    animationStart = millis();
    ledEnabled = true;
    ledColor = iColor;
}

void updateLedStrip() {
    unsigned long currentTime = millis();

    // Check if animation has expired
    if (currentTime - animationStart > animationDuration) {
        ledEnabled = false;
        showLedStrip(0, blackColor);
        return;
    }

    // Update animation every 50ms
    if (currentTime - lastAnimationTime > 50) {
        Serial.print("Turning on LED ");
        Serial.println(ledIndex);

        lastAnimationTime = currentTime;

        if (animationMoving) {
            ledIndex++;
        }
        ledIndex = ledIndex % LED_DISTANCE;

        if (ledEnabled) {
            showLedStripCurrentColor(ledIndex);
        }
    }
}

void go() {
    // Clear matrix
    ledMatrix.clear();
    ledMatrix.show();

    // Clear strip and show red pattern
    strip.clear();
    showLedStrip(0, redColor);

    // Onboard LED red during countdown
    onboardLed.setPixelColor(0, redColor);
    onboardLed.show();

    // Countdown sequence on 8x8 matrix (rows light up)
    uint16_t ranges[] = {0, 8, 16, 24, 32, 40, 48, 56};
    for (uint8_t i = 0; i < 8; i += 2) {
        showRange(ranges[i], 8, redColor);
        showRange(ranges[i + 1], 8, redColor);
        delay(1000);
    }

    // GO! - Turn matrix green
    ledMatrix.fill(greenColor, 0, MATRIX_NUM_LEDS);
    ledMatrix.show();

    // Onboard LED green for GO!
    onboardLed.setPixelColor(0, greenColor);
    onboardLed.show();

    // Start green moving animation on strip
    startAnimation(greenColor, 10000, true);

    // Run animation for 2 seconds
    unsigned long start = millis();
    while (millis() - start < 2000) {
        updateLedStrip();
    }

    // Fade out matrix from green (onboard LED stays green)
    for (int brightness = 50; brightness >= 0; brightness -= 2) {
        uint32_t fadeGreen = ledMatrix.Color(0, brightness, 0);
        ledMatrix.fill(fadeGreen, 0, MATRIX_NUM_LEDS);
        ledMatrix.show();
        delay(50);
    }
    ledMatrix.clear();
    ledMatrix.show();
}

void loop() {
    // Button A - Start countdown sequence
    if (digitalRead(BUTTON_A_PIN) == LOW && !buttonAWasPressed) {
        buttonAWasPressed = true;
        Serial.println("Button A pressed (GPIO19) - Starting countdown sequence");
        go();
        Serial.println("Countdown sequence complete");
    } else if (digitalRead(BUTTON_A_PIN) == HIGH) {
        buttonAWasPressed = false;
    }

    // Button B - Yellow flash while held
    if (digitalRead(BUTTON_B_PIN) == LOW) {
        Serial.println("Button B pressed - Yellow flash");
        BWasPressed = true;
        ledMatrix.fill(yellowColor, 0, MATRIX_NUM_LEDS);
        ledMatrix.show();
        delay(100);
        ledMatrix.clear();
        ledMatrix.show();
        delay(100);

        startAnimation(yellowColor, 1000, true);
    }
    else if (BWasPressed) {
        // When button B released - show red
        Serial.println("Button B released - Showing red");
        BWasPressed = false;
        ledMatrix.fill(redColor, 0, MATRIX_NUM_LEDS);
        ledMatrix.show();
        strip.clear();
        showLedStrip(0, redColor);
        startAnimation(redColor, 10000, false);
        delay(1000);
    }

    // Update strip animation
    updateLedStrip();
}
