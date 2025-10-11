// Example to control buttons on Adafruit TFT Feather
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789

int D0Pin = 0;
int D1Pin = 1;
int D2Pin = 2;

void setup(){
  Serial.begin(115200);
}
bool d0Pressed = false;
bool d1Pressed = false;
bool d2Pressed = false;

void loop(){
  // D0
  bool d0CurrentlyPressed = digitalRead(D0Pin) == LOW; // D0 is LOW when pressed, shared with boot
  if(d0CurrentlyPressed != d0Pressed)
  {
    d0Pressed = d0CurrentlyPressed;
    Serial.print("Button 0: ");
    if(d0CurrentlyPressed)
    {
      Serial.println("Pressed");
    }
    else
    {
      Serial.println("Not Pressed");
    }
  }

  // D1
  bool d1CurrentlyPressed = digitalRead(D1Pin) == HIGH; // D1 is High when pressed
  if(d1CurrentlyPressed != d1Pressed)
  {
    d1Pressed = d1CurrentlyPressed;
    Serial.print("Button 1: ");
    if(d1CurrentlyPressed)
    {
      Serial.println("Pressed");
    }
    else
    {
      Serial.println("Not Pressed");
    }
  }
  
  // D2
  bool d2CurrentlyPressed = digitalRead(D2Pin) == HIGH; // D2 is High when pressed
  if(d2CurrentlyPressed != d2Pressed)
  {
    d2Pressed = d2CurrentlyPressed;
    Serial.print("Button 2: ");
    if(d2CurrentlyPressed)
    {
      Serial.println("Pressed");
    }
    else
    {
      Serial.println("Not Pressed");
    }
  }
  delay(200);
}
