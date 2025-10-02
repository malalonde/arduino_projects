
#if 1
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Change 0x27 to whatever the scanner finds
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  lcd.init();       // initialize the lcd
  lcd.backlight();  // turn on backlight
  lcd.setCursor(0,0);
  lcd.print("Hello");
}

void loop() {}

#else
#include <Wire.h>

void setup() {
  Wire.begin();
  Serial.begin(9600);
  while (!Serial);
  Serial.println("\nI2C Scanner");
}

void loop() {
  byte error, address;
  int nDevices = 0;

  for(address = 1; address < 127; address++ ) {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0) {
      Serial.print("I2C device found at 0x");
      if (address<16) Serial.print("0");
      Serial.print(address,HEX);
      Serial.println(" !");
      nDevices++;
    }
  }
  if (nDevices == 0) Serial.println("No I2C devices found\n");
  else Serial.println("done\n");

  delay(5000);
}
#endif
