#include <Adafruit_NeoPixel.h>

#define PIN 12

#define NUMPIXELS 4

Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  pixels.begin();
}

void loop() {
  pixels.clear();
 for(int i=0; i<NUMPIXELS; i++) 
 {
    pixels.setPixelColor(i, pixels.Color(0, 150, 0));
    pixels.show();
    delay(500); // Pause before next pass through loop
  }
}

// input.onButtonPressed(Button.A, function () {
//     go()
// })
// function initialise () {
//     BWasPressed = 0
//     range = strip.range(0, 8)
//     range2 = strip.range(8, 8)
//     range3 = strip.range(16, 8)
//     range4 = strip.range(24, 8)
//     range5 = strip.range(32, 8)
//     range6 = strip.range(40, 8)
//     range7 = strip.range(48, 8)
//     range8 = strip.range(56, 8)
// }
// function go () {
//     strip.showColor(neopixel.colors(NeoPixelColors.Black))
//     range.showColor(neopixel.colors(NeoPixelColors.Red))
//     basic.pause(1000)
//     range2.showColor(neopixel.colors(NeoPixelColors.Red))
//     basic.pause(1000)
//     range3.showColor(neopixel.colors(NeoPixelColors.Red))
//     basic.pause(1000)
//     range4.showColor(neopixel.colors(NeoPixelColors.Red))
//     basic.pause(1000)
//     range5.showColor(neopixel.colors(NeoPixelColors.Red))
//     basic.pause(1000)
//     range6.showColor(neopixel.colors(NeoPixelColors.Red))
//     basic.pause(1000)
//     range7.showColor(neopixel.colors(NeoPixelColors.Red))
//     basic.pause(1000)
//     range8.showColor(neopixel.colors(NeoPixelColors.Red))
//     basic.pause(1000)
//     strip.showColor(neopixel.colors(NeoPixelColors.Green))
//     basic.pause(2000)
//     strip.showColor(neopixel.colors(NeoPixelColors.Black))
// }
// let range8: neopixel.Strip = null
// let range7: neopixel.Strip = null
// let range6: neopixel.Strip = null
// let range5: neopixel.Strip = null
// let range4: neopixel.Strip = null
// let range3: neopixel.Strip = null
// let range2: neopixel.Strip = null
// let range: neopixel.Strip = null
// let BWasPressed = 0
// let strip: neopixel.Strip = null
// strip = neopixel.create(DigitalPin.P1, 64, NeoPixelMode.RGB)
// strip.setBrightness(50)
// initialise()
// basic.forever(function () {
//     if (input.buttonIsPressed(Button.B)) {
//         BWasPressed = 1
//         strip.showColor(neopixel.colors(NeoPixelColors.Yellow))
//         basic.pause(100)
//         strip.showColor(neopixel.colors(NeoPixelColors.Black))
//         basic.pause(100)
//     } else if (BWasPressed) {
//         BWasPressed = 0
//         strip.showColor(neopixel.colors(NeoPixelColors.Red))
//     } else {
    	
//     }
// })

