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

// Use dedicated hardware SPI pins
Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

// Reset pin, MFIO pin
int resPin = A5;
int mfioPin = A3;

int D1Pin = 1;
int D2Pin = 2;

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

void drawHeart(int x, int y, uint16_t color) {
  tft.fillCircle(x - 4, y, 6, color);
  tft.fillCircle(x + 4, y, 6, color);
  int16_t xPoints[4] = {x - 10, x, x + 10, x};
  int16_t yPoints[4] = {y, y + 14, y, y};
  tft.fillTriangle(x - 10, y, x, y + 14, x + 10, y, color);
}

void setup(){

  Serial.begin(115200);

  // turn on backlite
  pinMode(TFT_BACKLITE, OUTPUT);
  digitalWrite(TFT_BACKLITE, HIGH);

  // turn on the TFT / I2C power supply
  pinMode(TFT_I2C_POWER, OUTPUT);
  digitalWrite(TFT_I2C_POWER, HIGH);
  delay(10);

  // initialize TFT
  tft.init(135, 240); // Init ST7789 240x135
  tft.setRotation(3);
  tft.fillScreen(ST77XX_BLACK);

  Serial.println(F("Initialized"));

  drawHeart(35, 70, ST77XX_RED);

  Wire.begin();
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

void loop(){

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
    
    // Slow it down or your heart rate will go up trying to keep up
    // with the flow of numbers
    delay(250); 

  static int count = 0;
  count++;
    Serial.println(count); 
  if(count % 10 && body.heartRate > 40 && body.heartRate < 190)
  {
    tft.setTextColor(ST77XX_RED,ST77XX_BLACK);
    tft.setTextWrap(true);
    tft.setTextSize(4);
    //tft.print("Heartrate: ");
    
    tft.setCursor(60, 60);
    tft.println(body.heartRate); 
  }

  static int d1Pressed = HIGH;
  int pressed = digitalRead(D1Pin);
  if(pressed != d1Pressed)
  {
    d1Pressed = pressed;
    tft.setCursor(0, 0);
    tft.setTextColor(ST77XX_RED,ST77XX_BLACK);
    tft.setTextSize(1);
    Serial.print("Button: ");
    tft.print("Button: ");
    if(pressed == HIGH)
    {
      Serial.print("Pressed     ");
      tft.print("1");
    }
    else
    {
      Serial.print("Not Pressed");
      tft.print("0");
    }
  }
}
