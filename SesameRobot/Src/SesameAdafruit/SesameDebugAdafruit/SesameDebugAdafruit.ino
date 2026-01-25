#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

// ======================================================================
// --- CONFIGURATION ---
// ======================================================================

// PCA9685 Servo FeatherWing driver
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

// PCA9685 servo pulse mapping (in pulse counts out of 4096)
#define SERVO_FREQ 50
#define SERVOMIN 150   // ~732us at 50Hz
#define SERVOMAX 600   // ~2929us at 50Hz

// Track which servos are "active" (for stop command)
bool servoActive[8] = {false, false, false, false, false, false, false, false};

// ======================================================================
// --- SETUP ---
// ======================================================================

void setup() {
  Serial.begin(115200);
  delay(1000);  // Give serial monitor time to connect

  Serial.println("\n\n-----------------------------------");
  Serial.println("   Sesame Motor Calibrator (PCA9685)");
  Serial.println("-----------------------------------");

  // I2C Init - uses default Qwiic pins on ESP32-S3 Feather
  Wire.begin();
  Serial.print("I2C initialized. SDA=");
  Serial.print(SDA);
  Serial.print(" SCL=");
  Serial.println(SCL);

  // I2C Scanner - find all devices on the bus
  Serial.println("\nScanning I2C bus...");
  byte deviceCount = 0;
  for (byte addr = 1; addr < 127; addr++) {
    Wire.beginTransmission(addr);
    byte error = Wire.endTransmission();
    if (error == 0) {
      Serial.print("  Device found at 0x");
      if (addr < 16) Serial.print("0");
      Serial.print(addr, HEX);
      if (addr == 0x3C || addr == 0x3D) Serial.print(" (SSD1306 OLED)");
      if (addr == 0x40) Serial.print(" (PCA9685 Servo)");
      Serial.println();
      deviceCount++;
    }
  }
  if (deviceCount == 0) {
    Serial.println("  No I2C devices found! Check wiring.");
  } else {
    Serial.print("  Total devices: ");
    Serial.println(deviceCount);
  }
  Serial.println();

  // Initialize PCA9685
  Serial.println("Initializing PCA9685 Servo FeatherWing at 0x40...");
  pwm.begin();
  pwm.setOscillatorFrequency(27000000);
  pwm.setPWMFreq(SERVO_FREQ);
  delay(10);
  Serial.println("PCA9685 initialized.");

  Serial.println("\n-----------------------------------");
  Serial.println("Commands:");
  Serial.println("1. id,angle   -> e.g. '0,90'");
  Serial.println("2. all,angle  -> e.g. 'all,90'");
  Serial.println("3. stop       -> Powers down all servos");
  Serial.println("4. scan       -> Rescan I2C bus");
  Serial.println("-----------------------------------");
  Serial.println("Status: Ready. Servos respond on channels 0-7.");
}

// ======================================================================
// --- MAIN LOOP ---
// ======================================================================

void loop() {
  if (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n');
    input.trim(); // Remove whitespace/newlines

    if (input.length() == 0) return;

    // --- Check for STOP command ---
    if (input.equalsIgnoreCase("stop")) {
      stopMotors();
      return;
    }

    // --- Check for SCAN command ---
    if (input.equalsIgnoreCase("scan")) {
      scanI2C();
      return;
    }

    // --- Parse "key,value" format ---
    int commaIndex = input.indexOf(',');

    if (commaIndex != -1) {
      String cmd = input.substring(0, commaIndex);
      String valStr = input.substring(commaIndex + 1);
      int angle = valStr.toInt();

      // Check Angle Limits
      if (angle < 0) angle = 0;
      if (angle > 180) angle = 180;

      // Check if command is "all" or a motor ID
      if (cmd.equalsIgnoreCase("all")) {
        moveAll(angle);
      } else {
        // Assume it's a number
        int motorId = cmd.toInt();
        // Basic check to ensure "0" wasn't actually text garbage
        if (motorId == 0 && cmd.charAt(0) != '0') {
           Serial.println("Error: Invalid Motor ID");
        } else {
           moveMotor(motorId, angle);
        }
      }

    } else {
      Serial.println("Error: Invalid format. Use 'id,angle', 'all,angle', 'stop', or 'scan'.");
    }
  }
}

// ======================================================================
// --- HELPER FUNCTIONS ---
// ======================================================================

void moveMotor(int id, int angle) {
  if (id < 0 || id > 7) {
    Serial.println("Error: Motor ID must be 0-7");
    return;
  }

  // Map 0-180 degrees to pulse length counts
  uint16_t pulse = map(angle, 0, 180, SERVOMIN, SERVOMAX);
  pwm.setPWM(id, 0, pulse);
  servoActive[id] = true;

  Serial.print("OK: Motor ");
  Serial.print(id);
  Serial.print(" -> ");
  Serial.print(angle);
  Serial.print(" deg (pulse=");
  Serial.print(pulse);
  Serial.println(")");
}

void moveAll(int angle) {
  Serial.print("Moving ALL to ");
  Serial.println(angle);
  for (int i = 0; i < 8; i++) {
    moveMotor(i, angle);
  }
}

void stopMotors() {
  Serial.println("Stopping all servos (setting PWM to 0)...");
  for (int i = 0; i < 8; i++) {
    pwm.setPWM(i, 0, 0);  // Turn off PWM signal
    servoActive[i] = false;
  }
  Serial.println("Servos are now OFF (no PWM signal).");
}

void scanI2C() {
  Serial.println("\nRescanning I2C bus...");
  byte deviceCount = 0;
  for (byte addr = 1; addr < 127; addr++) {
    Wire.beginTransmission(addr);
    byte error = Wire.endTransmission();
    if (error == 0) {
      Serial.print("  Device found at 0x");
      if (addr < 16) Serial.print("0");
      Serial.print(addr, HEX);
      if (addr == 0x3C || addr == 0x3D) Serial.print(" (SSD1306 OLED)");
      if (addr == 0x40) Serial.print(" (PCA9685 Servo)");
      Serial.println();
      deviceCount++;
    }
  }
  if (deviceCount == 0) {
    Serial.println("  No I2C devices found!");
  } else {
    Serial.print("  Total devices: ");
    Serial.println(deviceCount);
  }
}
