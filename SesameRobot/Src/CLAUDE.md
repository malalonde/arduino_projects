# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

SesameRobot is an ESP32-S3-based quadruped robot with 8-servo leg control via I2C PWM driver, WiFi captive portal web controller, and 128x64 OLED display for facial expressions.

## Build System

- **Toolchain**: Arduino IDE with ESP32 board support (Espressif Systems v2.0.0+)
- **Target Board**: Adafruit ESP32-S3 Feather with Adafruit Servo FeatherWing (PCA9685)
- **Serial Monitor**: 115200 baud

### Required Libraries
- `Adafruit_PWMServoDriver` - PCA9685 I2C PWM servo control
- `Adafruit_SSD1306` - OLED framebuffer rendering
- `Adafruit_GFX` - Graphics primitives
- `WiFi.h`, `WebServer.h`, `DNSServer.h` - Network stack

## Architecture

### Directory Structure
- `SesameAdafruit/` - **ACTIVE PROJECT** - Main development folder
  - `SesameAdafruit/` - Main robot firmware (ESP32-S3 Feather + FeatherWing)
  - `SesameDebugAdafruit/` - Motor tester/calibration utility for PCA9685
- `SesameOriginal/` - **REFERENCE ONLY** - Original ESP32 GPIO-based firmware
- `AdafruitServoExample/` - **REFERENCE ONLY** - Example code for PCA9685 library
- `AdafruitSSD1306Example/` - **REFERENCE ONLY** - Example code for SSD1306 library

### Key Files
- `SesameAdafruit/SesameAdafruit/SesameAdafruit.ino` - Main firmware
- `SesameAdafruit/SesameAdafruit/Faces.h` - PROGMEM bitmap arrays for face expressions
- `SesameAdafruit/SesameDebugAdafruit/SesameDebugAdafruit.ino` - Motor calibration utility

### Hardware Configuration
Servos connect to PCA9685 FeatherWing channels 0-7. OLED and servo driver share I2C bus via Qwiic/STEMMA QT:
```cpp
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();  // Default 0x40
#define OLED_I2C_ADDR 0x3C  // Some displays use 0x3D
```

### Web API Endpoints
- `/` - Main controller UI
- `/cmd?go=[forward|backward|left|right]` - Movement commands
- `/cmd?pose=[rest|stand|wave|dance|...]` - Animation poses
- `/getSettings`, `/setSettings` - JSON configuration

## Key Patterns

### Non-Blocking Control
Use `pressingCheck(String cmd, int ms)` instead of `delay()` for interruptible animations. This polls web server handlers during motion frames.

### I2C Bus
Both the OLED display and PCA9685 servo driver share the I2C bus. Default Qwiic pins are used on ESP32-S3 Feather (no manual pin configuration needed). Debug firmware includes I2C scanner to verify device connectivity.

### Memory Optimization
Face bitmaps (1024 bytes each) use `PROGMEM` to prevent SRAM exhaustion.

### Motor Brownout Prevention
`motorCurrentDelay` (default 20ms) staggers servo activation to prevent VCC rail collapse. Adjustable via web settings.

## Serial Commands (SesameDebugAdafruit)
- `id,angle` - Set single servo (e.g., "0,90")
- `all,angle` - Set all servos (e.g., "all,90")
- `stop` - Disable all servo PWM signals
- `scan` - Rescan I2C bus for connected devices

## Face Asset Pipeline
1. Create 128x64 image using Kaomoji/Emojicombos + jsPaint
2. Convert with image2cpp (Horizontal scaling, Arduino Code output)
3. Add to `Faces.h` as `PROGMEM const unsigned char` array
