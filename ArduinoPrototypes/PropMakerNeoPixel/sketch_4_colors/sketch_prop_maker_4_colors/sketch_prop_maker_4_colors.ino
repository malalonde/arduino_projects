#include <Adafruit_NeoPixel.h>


#ifdef USE_TINYUSB  // For Serial when selecting TinyUSB
#include <Adafruit_TinyUSB.h>
#endif

// NeoPixel strip length, in pixels
#define NUM_PIXELS 4
#define NEOPIXEL_PIN 21
#define LED_PIN 13   // onboard status LED on RP2040 Feather/PropMaker

// create a neopixel strip
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_PIXELS, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel stripOnBoard = Adafruit_NeoPixel(1, PIN_NEOPIXEL, NEO_GRB + NEO_KHZ800);

void setup() {
  Serial.begin(115200);
  Serial.println("\nProp-Maker Wing: NeoPixel");

  // Set power pin to output
  pinMode(PIN_EXTERNAL_POWER, OUTPUT);
  digitalWrite(PIN_EXTERNAL_POWER, HIGH);

  // This initializes the NeoPixel library.
  strip.begin();

  strip.setBrightness(255);

  // Onboard LED
  pinMode(LED_PIN, OUTPUT);  // set pin as output
  digitalWrite(LED_PIN, HIGH);  // turn LED on

  // Onboard Color LED
  stripOnBoard.begin();
  stripOnBoard.setBrightness(4);
  stripOnBoard.setPixelColor(0, stripOnBoard.Color(128,0,128));
  stripOnBoard.show();

  // Colors for microscope
  strip.setPixelColor(0, stripOnBoard.Color(50,50,50));
  strip.setPixelColor(1, stripOnBoard.Color(50,0,0));
  strip.setPixelColor(2, stripOnBoard.Color(0,50,0));
  strip.setPixelColor(3, stripOnBoard.Color(0,0,50));
  strip.show();
}


void loop()
{
}

