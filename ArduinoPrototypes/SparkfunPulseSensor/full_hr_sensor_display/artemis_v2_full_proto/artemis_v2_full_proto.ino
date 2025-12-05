/*
 This example sketch gives you exactly what the SparkFun Pulse Oximiter and
 Heart Rate Monitor is designed to do: read heart rate and blood oxygen levels.
 This board requires I-squared-C connections but also connections to the reset
 and mfio pins. When using the device keep LIGHT and CONSISTENT pressure on the
 sensor. Otherwise you may crush the capillaries in your finger which results
 in bad or no results. A summary of the hardware connections are as follows: 
 SDA -> SDA
 SCL -> SCL
 RESET -> PIN 4
 MFIO -> PIN 5

 Author: Elias Santistevan
 Date: 8/2019
 SparkFun Electronics

 If you run into an error code check the following table to help diagnose your
 problem: 
 1 = Unavailable Command
 2 = Unavailable Function
 3 = Data Format Error
 4 = Input Value Error
 5 = Try Again
 255 = Error Unknown
*/

#include <SparkFun_Bio_Sensor_Hub_Library.h>
#include <Wire.h>

#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
#include <SPI.h>


#include <Adafruit_DRV2605.h>

Adafruit_DRV2605 drv;


int D0Pin = 0;
int D1Pin = 1;
int D2Pin = 2;

// Reset pin, MFIO pin
int resPin = A3;
int mfioPin = A5;

// Takes address, reset pin, and MFIO pin.
SparkFun_Bio_Sensor_Hub bioHub(resPin, mfioPin); 

bioData body;  
// ^^^^^^^^^
// What's this!? This is a type (like int, byte, long) unique to the SparkFun
// Pulse Oximeter and Heart Rate Monitor. Unlike those other types it holds
// specific information on your heartrate and blood oxygen levels. BioData is
// actually a specific kind of type, known as a "struct". 
// You can choose another variable name other than "body", like "blood", or
// "readings", but I chose "body". Using this "body" varible in the 
// following way gives us access to the following data: 
// body.heartrate  - Heartrate
// body.confidence - Confidence in the heartrate value
// body.oxygen     - Blood oxygen level
// body.status     - Has a finger been sensed?



// Use dedicated hardware SPI pins
Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

// ------------- UTILITIES ----------------
// Draw a "wide" line using two filled triangles (fast & anti-jaggies free)
void drawWideLine(int x0, int y0, int x1, int y1, float w, uint16_t color) {
  float dx = x1 - x0;
  float dy = y1 - y0;
  float len = sqrtf(dx*dx + dy*dy);
  if (len < 0.5f) return;

  // Unit perpendicular vector scaled by half the width
  float px = (-dy / len) * (w * 0.5f);
  float py = ( dx / len) * (w * 0.5f);

  // Four corners of the thick segment
  int xA = (int)roundf(x0 + px), yA = (int)roundf(y0 + py);
  int xB = (int)roundf(x0 - px), yB = (int)roundf(y0 - py);
  int xC = (int)roundf(x1 + px), yC = (int)roundf(y1 + py);
  int xD = (int)roundf(x1 - px), yD = (int)roundf(y1 - py);

  // Two triangles to make a quad
  tft.fillTriangle(xA, yA, xB, yB, xC, yC, color);
  tft.fillTriangle(xC, yC, xB, yB, xD, yD, color);

  // Round end caps
  tft.fillCircle(x0, y0, (int)ceilf(w * 0.5f), color);
  tft.fillCircle(x1, y1, (int)ceilf(w * 0.5f), color);
}

// Convenience polyline with uniform width
void drawWidePolyline(const int* xs, const int* ys, int n, float w, uint16_t color) {
  for (int i = 0; i < n - 1; ++i) {
    drawWideLine(xs[i], ys[i], xs[i+1], ys[i+1], w, color);
  }
}

void setup(){

  Serial.begin(115200);
//screnn
// turn on backlite
  pinMode(TFT_BACKLITE, OUTPUT);
  digitalWrite(TFT_BACKLITE, HIGH);

  // turn on the TFT / I2C power supply
  pinMode(TFT_I2C_POWER, OUTPUT);
  digitalWrite(TFT_I2C_POWER, HIGH);
  delay(1000);

  // initialize TFT
  tft.init(135, 240); // Init ST7789 240x135
  tft.setRotation(3);
  tft.fillScreen(ST77XX_BLACK);

  // Set text size and color
   tft.setTextColor(ST77XX_WHITE, ST77XX_BLACK);
   tft.setTextSize(3);

 // Colors
  const uint16_t BLACK = ST77XX_BLACK;
  const uint16_t RED   = ST77XX_RED;   // 0xF800
  const uint16_t BLUE  = ST77XX_BLUE;  // 0x001F

  //tft.fillScreen(BLACK);

  // Screen size (works for any init size)
  int W = 240;
  int H = 135;

  // ------- Geometry (proportional to screen) -------
  // Red zig-zag polyline points
  int yMid = (int)(H * 0.58f);
  int xs[] = {
    (int)(W * 0.00f),
    (int)(W * 0.25f),
    (int)(W * 0.35f),
    (int)(W * 0.55f),
    (int)(W * 0.75f),
    (int)(W * 1.00f) - 1
  };
  int ys[] = {
    yMid,                 // start left mid
    yMid,                 // short flat
    (int)(H * 0.85f),     // deep V
    (int)(H * 0.05f),     // apex
    yMid,                 // back to mid
    yMid                  // long flat to right
  };
  const int NPTS = sizeof(xs) / sizeof(xs[0]);

  float redStroke  = max(6, (int)(min(W, H) * 0.06f));   // ~6% of min dimension
  float blueStroke = max(4, (int)(min(W, H) * 0.045f));  // a bit thinner

  // Draw red polyline
  drawWidePolyline(xs, ys, NPTS, redStroke, RED);

  // Blue crossbar of the "A"
  int yCross  = (int)(H * 0.42f);      // vertical position of the bar
  int xCrossL = (int)(W * 0.48f);
  int xCrossR = (int)(W * 0.66f);
  drawWideLine(xCrossL, yCross, xCrossR, yCross, blueStroke, BLUE);

       tft.println("Artemis");

//end screen

  Wire.begin(SDA, SCL);
  Wire.setClock(100000);


  Serial.println("Initializing DRV2605L...");

  if (!drv.begin()) {
    Serial.println("Failed to find DRV2605");
    while (1); // Halt if device not found
  }

  drv.selectLibrary(1);  // 1 = ERM motor; use 6 for LRA if needed

  delay(1000); 

  int result = bioHub.begin();
  if (result == 0) // Zero errors!
    Serial.println("Sensor started!");
  else
    Serial.println("Could not communicate with the sensor!");
 
  Serial.println("Configuring Sensor...."); 
  int error = bioHub.configBpm(MODE_ONE); // Configuring just the BPM settings. 
  if(error == 0){ // Zero errors!
    Serial.println("Sensor configured.");
  }
  else {
    Serial.println("Error configuring sensor.");
    Serial.print("Error: "); 
    Serial.println(error); 
  }

  // Data lags a bit behind the sensor, if you're finger is on the sensor when
  // it's being configured this delay will give some time for the data to catch
  // up. 
  Serial.println("Loading up the buffer with data....");
  delay(4000); 


}
bool d0Pressed = false;
bool d1Pressed = false;
bool d2Pressed = false;

int status = 0;
int confidence =0;
int vibrateStartTime = millis() - 300'000;

bool sensorEnabled = true;


void loop()
{
  // D1
  bool d1CurrentlyPressed = digitalRead(D1Pin) == HIGH; // D1 is High when pressed
  if(d1CurrentlyPressed != d1Pressed)
  {
    d1Pressed = d1CurrentlyPressed;
    Serial.print("Button 1: ");
    if(d1CurrentlyPressed)
    {
      vibrateStartTime = millis()-300'000;
      Serial.println("Pressed");
    }
    else
    {
      Serial.println("Not Pressed");
    }
  }

  bool d2CurrentlyPressed = digitalRead(D2Pin) == HIGH; // D2 is High when pressed
  if(d2CurrentlyPressed != d2Pressed)
  {
    d2Pressed = d2CurrentlyPressed;
    Serial.print("Button 2: ");
    if(d2CurrentlyPressed)
    {
      Serial.println("Pressed");
      vibrateStartTime = millis();
    }
    else
    {
      Serial.println("Not Pressed");
    }
  }


  if(vibrateStartTime + 30'000 > (long)millis())
  {
    if(sensorEnabled)
    {
      // // Pause bio sensor by pulling MFIO LOW
      //   bioHub.max30101Control(DISABLE);
      //   delay(10);
    }
    
    // First beat: soft bump
    drv.setWaveform(0, 3);   // 3 = Soft bump
    drv.setWaveform(1, 0);   // End sequence
    drv.go();                // Play it
    delay(100);              // Small pause
    
      Serial.println("BZZZ");

    //Second beat: soft bump again
    drv.setWaveform(0, 1);   // First strong click
    drv.setWaveform(1, 1);   // Second strong click
    drv.setWaveform(2, 0);   // End
    drv.go();
    int bpm= 60;
    delay(60000/bpm);     
  }
  else
  {
    if(!sensorEnabled) 
    {
        // sensorEnabled = true;
        // bioHub.max30101Control(ENABLE);
        // delay(10);
    }
    
    //digitalWrite(mfioPin, LOW);   // normal application mode
        // Information from the readBpm function will be saved to our "body"
    // variable.  
    body = bioHub.readBpm();
    Serial.print("Heartrate: ");
    Serial.println(body.heartRate); 
    Serial.print("Confidence: ");
    Serial.println(body.confidence); 
    Serial.print("Oxygen: ");
    Serial.println(body.oxygen); 
    Serial.print("Status: ");
    Serial.println(body.status); 
    if (body.confidence>85){
      Serial.println("Update Screen");

      tft.setCursor(0, 0);
   tft.setTextSize(3);
       tft.print("bpm: ");
       tft.print(body.heartRate);
       tft.print("  ");
       }

    if(status != body.status)
    {
      status = body.status;
      tft.setCursor(0, 120);
   tft.setTextSize(2);
      tft.print(body.status);
    }

    if(confidence != body.confidence)
    {
      confidence = body.confidence;
      tft.setCursor(20, 120);
   tft.setTextSize(2);
      tft.print(body.confidence);
    }
    // Slow it down or your heart rate will go up trying to keep up
    // with the flow of numbers
    delay(250); 
  }
}
