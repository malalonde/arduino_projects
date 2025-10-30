#include <Adafruit_NeoPixel.h>


// How many NeoPixels are attached to the Arduino?
#define LED_COUNT 50 // Popular NeoPixel ring size

Adafruit_NeoPixel strip(170, PIN_EXTERNAL_NEOPIXELS, NEO_GRB + NEO_KHZ800);

uint32_t halloweenColors[] = {
  strip.Color(255, 69, 0),     // Orange
  strip.Color(128, 0, 128),    // Purple
  strip.Color(255, 0, 0),      // Red
  strip.Color(255, 255, 0)     // Yellow (optional)
};
int colorCount = sizeof(halloweenColors) / sizeof(uint32_t);

void setPixelOffsetted(int i, uint32_t color)
{
  strip.setPixelColor(i * 2 + 20, color);
}
void setPixelOffsetted(int i, uint32_t r, uint32_t g, uint32_t b)
{
  strip.setPixelColor(i * 2 + 20, r, g, b);
}
void setup() {
  pinMode(PIN_EXTERNAL_POWER, OUTPUT);
  digitalWrite(PIN_EXTERNAL_POWER, HIGH);
  strip.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
   strip.setBrightness(10);
   randomSeed(analogRead(0)); // Randomize
}

void loop() {
  //randomFlicker();    // Flicker random lights
  
 // Randomly pick one of the four effects
  int mode = random(0, 4);
  switch (mode) {
    case 0: randomFlicker();  break;
    case 1: colorPulse();     break;
    case 2: colorChase();     break;
    case 3: candleFlicker();  break;
  }
}

// ----- RANDOM FLICKER -----
void randomFlicker() {
  for (int i = 0; i < LED_COUNT; i++) {
    int led = random(0, LED_COUNT);
    uint32_t color = halloweenColors[random(0, colorCount)];
    setPixelOffsetted(led , color);
    strip.show();
    delay(random(30, 120));
    setPixelOffsetted(led, 0);
  }
  strip.show();
}

// ----- COLOR PULSE -----
void colorPulse() {
  uint32_t color = halloweenColors[random(0, colorCount)];
  uint8_t r = (color >> 16) & 0xFF;
  uint8_t g = (color >> 8) & 0xFF;
  uint8_t b = color & 0xFF;

  for (int brightness = 0; brightness <= 255; brightness += 5) {
    for (int i = 0; i < LED_COUNT; i++) {
      setPixelOffsetted(i , (r * brightness / 255), (g * brightness / 255), (b * brightness / 255));
    }
    strip.show();
    delay(15);
  }
  for (int brightness = 255; brightness >= 0; brightness -= 5) {
    for (int i = 0; i < LED_COUNT; i++) {
      setPixelOffsetted(i , (r * brightness / 255), (g * brightness / 255), (b * brightness / 255));
    }
    strip.show();
    delay(15);
  }
}

// ----- COLOR CHASE -----
void colorChase() {
  uint32_t color = halloweenColors[random(0, colorCount)];
  for (int i = 0; i < LED_COUNT; i++) {
    setPixelOffsetted(i , color);
    if (i > 0) setPixelOffsetted(i - 1 , 0);
    strip.show();
    delay(50);
  }
  strip.clear();
  strip.show();
}

// ----- CANDLE FLICKER -----
void candleFlicker() {
  for (int i = 0; i < 100; i++) {
    // Base color (warm orange/yellow)
    int r = 255;
    int g = random(100, 180);  // fluctuate green a bit
    int b = random(0, 30);     // very little blue

    // Flicker brightness (simulate flame movement)
    int flicker = random(-20, 20);
    int brightness = constrain(200 + flicker, 150, 255);

    for (int j = 0; j < LED_COUNT; j++) {
      setPixelOffsetted(j ,
        (r * brightness) / 255,
        (g * brightness) / 255,
        (b * brightness) / 255);
    }
    strip.show();
    delay(random(30, 80)); // random flicker speed
  }
  strip.clear();
  strip.show();
}