#include <Adafruit_NeoPixel.h>


#ifdef USE_TINYUSB  // For Serial when selecting TinyUSB
#include <Adafruit_TinyUSB.h>
#endif

// NeoPixel strip length, in pixels
#define LED_COUNT 200
#define NEOPIXEL_PIN 21
#define LED_PIN 13   // onboard status LED on RP2040 Feather/PropMaker

// create a neopixel strip
Adafruit_NeoPixel strip = Adafruit_NeoPixel(LED_COUNT, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel stripOnBoard = Adafruit_NeoPixel(1, PIN_NEOPIXEL, NEO_GRB + NEO_KHZ800);


#define FADE_DURATION 5000 // Total time for pattern to move down the strip (5 seconds)
// Calculate the necessary delay to complete 200 steps in 5000ms
#define STEP_DELAY (FADE_DURATION / LED_COUNT) 

// Define Light Red and Light Green
// Light Red (e.g., 50% brightness red): R=128, G=0, B=0
const uint32_t LIGHT_RED = strip.Color(50, 0, 0); 
// Light Green (e.g., 50% brightness green): R=0, G=128, B=0
const uint32_t LIGHT_GREEN = strip.Color(0, 50, 0); 
// Black/Off
const uint32_t OFF = strip.Color(0, 0, 0); 

unsigned long lastUpdate = 0;
int currentPixel = 0; // The starting point of the 3-LED pattern

void setup() {
  Serial.begin(115200);
  Serial.println("\nProp-Maker Wing: NeoPixel");

  // Set power pin to output
  pinMode(PIN_EXTERNAL_POWER, OUTPUT);
  digitalWrite(PIN_EXTERNAL_POWER, HIGH);

  // This initializes the NeoPixel library.
  strip.begin();

  strip.setBrightness(20);

  // Onboard LED
  pinMode(LED_PIN, OUTPUT);  // set pin as output
  digitalWrite(LED_PIN, HIGH);  // turn LED on

  // Onboard Color LED
  stripOnBoard.begin();
  stripOnBoard.setBrightness(4);
  stripOnBoard.setPixelColor(0, stripOnBoard.Color(128,0,128));
  stripOnBoard.show();
}

void loop() {
  unsigned long currentTime = millis();

  // Check if it's time to advance the pattern
  if (currentTime - lastUpdate >= STEP_DELAY) {
    lastUpdate = currentTime;

    // 1. Clear the strip before drawing the new frame (optional, but ensures clean movement)
    // strip.clear(); // Removing this makes the trail persist, which you might like!

    // 2. Iterate through all 200 LEDs and determine their color based on position
    for(int i=0; i < LED_COUNT; i++) {
      
      // Calculate the 'phase' of the LED relative to the moving pattern head
      // The "+ 1" and "% 3" groups the LEDs into 3 repeating states.
      // The 'currentPixel' counter makes the pattern move forward.
      int phase = (i - currentPixel + LED_COUNT) % 4; 

      uint32_t color;
      
      // Assign colors based on the phase: (Light Red -> Light Green -> Off)
      if (phase == 0) {
        color = LIGHT_RED;
      } else if (phase == 1) {
        color = OFF;
      } else if (phase == 2) {
        color = LIGHT_GREEN;
      } else { // phase == 3
        color = OFF; // Or use OFF to make it a chasing pattern
      }

      strip.setPixelColor(i, color);
    }
    
    // 3. Show the new frame
    strip.show();
    
    // 4. Advance the pattern head
    currentPixel++;

    // 5. Check if the pattern has completed one full loop
    if (currentPixel >= LED_COUNT) {
      currentPixel = 0; // Reset to start the pattern loop again
    }
  }

  delay(500);
}

