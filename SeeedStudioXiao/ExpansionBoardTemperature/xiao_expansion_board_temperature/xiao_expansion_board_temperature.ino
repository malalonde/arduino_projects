#include <Arduino.h>
#include <U8x8lib.h>
#include <Wire.h>
#include "DHT.h"

U8X8_SSD1306_128X64_NONAME_HW_I2C u8x8(/* clock=*/ SCL, /* data=*/ SDA, /* reset=*/ U8X8_PIN_NONE);   // OLEDs without Reset of the Display

DHT dht(DHT20); 

void setup() {
  Serial.begin(115200);
  Serial.println("DHTxx test!");
  Wire.begin();
  dht.begin();

  u8x8.begin();
  //u8x8.setFlipMode(1);   // Enable (1) and disbale (0) 180 degree rotation of the display content
}

void loop() {
  float temp_hum_val[2] = {0};
  if (!dht.readTempAndHumidity(temp_hum_val)) {
      Serial.print("Humidity: ");
      Serial.print(temp_hum_val[0]);
      Serial.print(" %\t");
      Serial.print("Temperature: ");
      Serial.print(temp_hum_val[1]);
      Serial.println(" *C");
  } else {
      Serial.println("Failed to get temprature and humidity value.");
  }

  u8x8.setFont(u8x8_font_chroma48medium8_r);
  u8x8.setCursor(0, 0);
  u8x8.print("Temp: ");
  u8x8.print(temp_hum_val[1]);

  
  u8x8.setCursor(0, 20);
  u8x8.print("Humidity: ");
  u8x8.print(temp_hum_val[0]);

  delay(200);
}