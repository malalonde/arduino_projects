#include <Wire.h>
#include "MAX30105.h"
#include "heartRate.h"  // from SparkFun library
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>

// TFT pins (pre-wired for Feather ESP32-S2 TFT)
#define TFT_CS   10
#define TFT_RST  9
#define TFT_DC   8
Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

// MAX3010x sensor
MAX30105 particleSensor;

long lastBeat = 0;
float beatsPerMinute;
int beatAvg;
const int avgSize = 4;
int beatHistory[avgSize];
int beatIndex = 0;

// Graph
int xPos = 0;

void setup()
{
  Serial.begin(115200);
  Wire.begin();

  // Init TFT
  tft.init(240, 135);
  tft.setRotation(3);
  tft.fillScreen(ST77XX_BLACK);
  tft.setTextWrap(false);
  tft.setCursor(10, 10);
  tft.setTextColor(ST77XX_GREEN);
  tft.setTextSize(2);
  tft.println("SparkFun Oximeter");

  // Init MAX3010x
  if (!particleSensor.begin(Wire, I2C_SPEED_STANDARD))
  {
    Serial.println("MAX3010x not found. Check wiring!");
    while (1);
  }

  // Configure sensor
  particleSensor.setup(); // use default settings
  particleSensor.setPulseAmplitudeRed(0x0A); // Turn Red LED low
  particleSensor.setPulseAmplitudeGreen(0);  // Turn off Green LED
}

void loop()
{
  // Read IR value (raw pulse waveform)
  long irValue = particleSensor.getIR();

  if (checkForBeat(irValue) == true)
  {
    // We sensed a beat!
    long delta = millis() - lastBeat;
    lastBeat = millis();

    float bpm = 60 / (delta / 1000.0);

    if (bpm > 20 && bpm < 200)
    {
      beatHistory[beatIndex] = bpm;
      beatIndex = (beatIndex + 1) % avgSize;

      int sum = 0;
      for (int i = 0; i < avgSize; i++) sum += beatHistory[i];
      beatAvg = sum / avgSize;

      Serial.print("BPM=");
      Serial.println(beatAvg);

      // Update TFT
      tft.fillRect(10, 40, 200, 40, ST77XX_BLACK);
      tft.setCursor(10, 40);
      tft.setTextColor(ST77XX_RED);
      tft.setTextSize(4);
      tft.print(beatAvg);
    }
  }

  // Draw waveform
  int y = map(irValue, 5000, 200000, 120, 60); // adjust based on readings
  if (y < 60) y = 60;
  if (y > 120) y = 120;

  tft.drawPixel(xPos, y, ST77XX_WHITE);

  xPos++;
  if (xPos >= 240) {
    xPos = 0;
    tft.fillRect(0, 60, 240, 75, ST77XX_BLACK); // clear waveform area
  }
}
