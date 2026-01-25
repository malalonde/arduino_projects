
#include <Arduino.h>
#include <U8g2lib.h>
#include <SPI.h>
#include <Wire.h>

#define USE_U8G2_DISPLAY 0

#if USE_U8G2_DISPLAY //Grove Oled Display
U8G2_SH1107_SEEED_128X128_1_SW_I2C display(U8G2_R0, /* clock=*/ SCL, /* data=*/ SDA, /* reset=*/ U8X8_PIN_NONE);
#else // Starter Kit, expansion board, Display
U8G2_SSD1306_128X64_NONAME_1_HW_I2C display(U8G2_R0, /* clock=*/ SCL, /* data=*/ SDA, /* reset=*/ U8X8_PIN_NONE);   // OLEDs without Reset of the Display
#endif
const int GSR=A0;
int sensorValue=0;
int gsr_average=0;

void setup(){
  Serial.begin(9600);
  
  display.begin();
  display.setFont(u8g2_font_lubB18_tf );
}

void loop(){
  long sum=0;
  for(int i=0;i<10;i++)           //Average the 10 measurements to remove the glitch
      {
      sensorValue=analogRead(GSR);
      #if defined(ESP32) // If running on esp32, divide by 4 to go from 4095 max reading to 1023
      sensorValue /= 4;
      #endif
      sum += sensorValue;
      delay(5);
      }
   gsr_average = sum/10;
   Serial.println(gsr_average);

  display.firstPage();
  do {
    display.drawStr(12,32,"GSR:");
    char buf[10]; // Buffer must be large enough to hold the number as a string
    sprintf(buf, "%d", gsr_average); // Convert integer to a C-string
    display.drawStr(12, 64, buf);
  } while ( display.nextPage() );

   delay(100);
}
