#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

// ======================================================================
// --- CONFIGURATION ---
// ======================================================================

// PCA9685 Servo FeatherWing driver
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

#define SERVO_FREQ 50

// Oscillator frequency - try 25000000 if 27000000 doesn't work
uint32_t oscFreq = 27000000;  // Can be changed with 'osc' command

// PCA9685 pulse mapping (in pulse counts out of 4096)
// At 50Hz (20ms period), each count = 20000us / 4096 = 4.88us
//
// Common servo pulse ranges:
//   Standard:  1000-2000us  -> counts 205-410
//   Extended:  500-2500us   -> counts 102-512
//   Original:  732-2929us   -> counts 150-600
//
uint16_t servoMin = 102;   // ~500us  - safe minimum
uint16_t servoMax = 512;   // ~2500us - safe maximum

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
  initPWM();

  printHelp();
}

void initPWM() {
  Serial.println("\nInitializing PCA9685 Servo FeatherWing at 0x40...");
  pwm.begin();
  pwm.setOscillatorFrequency(oscFreq);
  pwm.setPWMFreq(SERVO_FREQ);
  delay(10);
  Serial.print("  Oscillator: ");
  Serial.print(oscFreq / 1000000);
  Serial.println(" MHz");
  Serial.print("  PWM Freq: ");
  Serial.print(SERVO_FREQ);
  Serial.println(" Hz");
  Serial.print("  Pulse range: ");
  Serial.print(servoMin);
  Serial.print("-");
  Serial.print(servoMax);
  Serial.print(" counts (");
  Serial.print(servoMin * 20000 / 4096);
  Serial.print("-");
  Serial.print(servoMax * 20000 / 4096);
  Serial.println(" us)");
  Serial.println("PCA9685 ready.");
}

void printHelp() {
  Serial.println("\n-----------------------------------");
  Serial.println("Commands:");
  Serial.println("  id,angle    -> e.g. '0,90' (0-180 degrees)");
  Serial.println("  all,angle   -> e.g. 'all,90'");
  Serial.println("  raw,id,val  -> e.g. 'raw,0,307' (direct PWM count)");
  Serial.println("  stop        -> Turn off all servos");
  Serial.println("  scan        -> Rescan I2C bus");
  Serial.println("  range,min,max -> e.g. 'range,150,600'");
  Serial.println("  osc,freq    -> e.g. 'osc,25' (MHz)");
  Serial.println("  status      -> Show current settings");
  Serial.println("  help        -> Show this help");
  Serial.println("-----------------------------------");
  Serial.println("Tip: If servo spins continuously, try:");
  Serial.println("  1. osc,25   (change oscillator to 25MHz)");
  Serial.println("  2. range,205,410  (standard 1000-2000us)");
  Serial.println("-----------------------------------");
}

// ======================================================================
// --- MAIN LOOP ---
// ======================================================================

void loop() {
  if (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n');
    input.trim(); // Remove whitespace/newlines

    if (input.length() == 0) return;

    // --- Single word commands ---
    if (input.equalsIgnoreCase("stop")) {
      stopMotors();
      return;
    }
    if (input.equalsIgnoreCase("scan")) {
      scanI2C();
      return;
    }
    if (input.equalsIgnoreCase("status")) {
      printStatus();
      return;
    }
    if (input.equalsIgnoreCase("help")) {
      printHelp();
      return;
    }

    // --- Parse comma-separated commands ---
    int comma1 = input.indexOf(',');
    if (comma1 == -1) {
      Serial.println("Error: Invalid format. Type 'help' for commands.");
      return;
    }

    String cmd = input.substring(0, comma1);
    String rest = input.substring(comma1 + 1);

    // --- raw,id,value - send raw PWM count ---
    if (cmd.equalsIgnoreCase("raw")) {
      int comma2 = rest.indexOf(',');
      if (comma2 != -1) {
        int id = rest.substring(0, comma2).toInt();
        int val = rest.substring(comma2 + 1).toInt();
        sendRawPWM(id, val);
      } else {
        Serial.println("Error: Use 'raw,id,value'");
      }
      return;
    }

    // --- range,min,max - set pulse range ---
    if (cmd.equalsIgnoreCase("range")) {
      int comma2 = rest.indexOf(',');
      if (comma2 != -1) {
        servoMin = rest.substring(0, comma2).toInt();
        servoMax = rest.substring(comma2 + 1).toInt();
        Serial.print("Pulse range set to ");
        Serial.print(servoMin);
        Serial.print("-");
        Serial.print(servoMax);
        Serial.print(" counts (");
        Serial.print(servoMin * 20000 / 4096);
        Serial.print("-");
        Serial.print(servoMax * 20000 / 4096);
        Serial.println(" us)");
      } else {
        Serial.println("Error: Use 'range,min,max'");
      }
      return;
    }

    // --- osc,freq - set oscillator frequency ---
    if (cmd.equalsIgnoreCase("osc")) {
      int mhz = rest.toInt();
      if (mhz >= 20 && mhz <= 30) {
        oscFreq = mhz * 1000000UL;
        initPWM();
      } else {
        Serial.println("Error: Frequency should be 20-30 MHz");
      }
      return;
    }

    // --- all,angle ---
    if (cmd.equalsIgnoreCase("all")) {
      int angle = constrain(rest.toInt(), 0, 180);
      moveAll(angle);
      return;
    }

    // --- id,angle ---
    int motorId = cmd.toInt();
    if (motorId == 0 && cmd.charAt(0) != '0') {
      Serial.println("Error: Invalid command. Type 'help' for commands.");
      return;
    }
    int angle = constrain(rest.toInt(), 0, 180);
    moveMotor(motorId, angle);
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
  uint16_t pulse = map(angle, 0, 180, servoMin, servoMax);
  pwm.setPWM(id, 0, pulse);
  servoActive[id] = true;

  Serial.print("OK: Motor ");
  Serial.print(id);
  Serial.print(" -> ");
  Serial.print(angle);
  Serial.print(" deg (pulse=");
  Serial.print(pulse);
  Serial.print(" = ~");
  Serial.print(pulse * 20000 / 4096);
  Serial.println(" us)");
}

void sendRawPWM(int id, int value) {
  if (id < 0 || id > 7) {
    Serial.println("Error: Motor ID must be 0-7");
    return;
  }
  if (value < 0 || value > 4095) {
    Serial.println("Error: PWM value must be 0-4095");
    return;
  }

  pwm.setPWM(id, 0, value);
  servoActive[id] = true;

  Serial.print("OK: Motor ");
  Serial.print(id);
  Serial.print(" -> raw PWM ");
  Serial.print(value);
  Serial.print(" (~");
  Serial.print(value * 20000 / 4096);
  Serial.println(" us)");
}

void printStatus() {
  Serial.println("\n--- Current Settings ---");
  Serial.print("Oscillator: ");
  Serial.print(oscFreq / 1000000);
  Serial.println(" MHz");
  Serial.print("PWM Freq: ");
  Serial.print(SERVO_FREQ);
  Serial.println(" Hz");
  Serial.print("Pulse range: ");
  Serial.print(servoMin);
  Serial.print("-");
  Serial.print(servoMax);
  Serial.print(" counts (");
  Serial.print(servoMin * 20000 / 4096);
  Serial.print("-");
  Serial.print(servoMax * 20000 / 4096);
  Serial.println(" us)");
  Serial.println("\nAngle to pulse mapping:");
  Serial.print("  0 deg   -> ");
  Serial.print(servoMin);
  Serial.print(" (~");
  Serial.print(servoMin * 20000 / 4096);
  Serial.println(" us)");
  Serial.print("  90 deg  -> ");
  uint16_t mid = (servoMin + servoMax) / 2;
  Serial.print(mid);
  Serial.print(" (~");
  Serial.print(mid * 20000 / 4096);
  Serial.println(" us)");
  Serial.print("  180 deg -> ");
  Serial.print(servoMax);
  Serial.print(" (~");
  Serial.print(servoMax * 20000 / 4096);
  Serial.println(" us)");
  Serial.println("------------------------");
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
