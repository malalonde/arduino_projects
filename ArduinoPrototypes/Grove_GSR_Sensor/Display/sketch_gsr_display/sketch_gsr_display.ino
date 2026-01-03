
#include <Arduino.h>
#include <U8g2lib.h>
#include <SPI.h>
#include <Wire.h>
 
U8G2_SH1107_SEEED_128X128_1_SW_I2C u8g2(U8G2_R0, /* clock=*/ SCL, /* data=*/ SDA, /* reset=*/ U8X8_PIN_NONE);

const int GSR=A3;
int sensorValue=0;
int gsr_average=0;

void setup(){
  Serial.begin(9600);
  
  u8g2.begin();
  u8g2.setFont(u8g2_font_lubB18_tf );
}

void loop(){
  long sum=0;
  for(int i=0;i<10;i++)           //Average the 10 measurements to remove the glitch
      {
      sensorValue=analogRead(GSR);
      sum += sensorValue;
      delay(5);
      }
   gsr_average = sum/10;
   Serial.println(gsr_average);

  u8g2.firstPage();
  do {
    u8g2.drawStr(12,32,"GSR:");
    char buf[10]; // Buffer must be large enough to hold the number as a string
    sprintf(buf, "%d", gsr_average); // Convert integer to a C-string
    u8g2.drawStr(12, 64, buf);
  } while ( u8g2.nextPage() );

   delay(100);
}
