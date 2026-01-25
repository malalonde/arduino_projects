# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

SesameRobot is an ESP32-based quadruped robot with 8-servo leg control, WiFi captive portal web controller, and 128x64 OLED display for facial expressions.

## Build System

- **Toolchain**: Arduino IDE with ESP32 board support (Espressif Systems v2.0.0+)
- **Supported Boards**: Lolin S2 Mini, ESP32-WROOM32 DevKitC (configurable via pin definitions)
- **Serial Monitor**: 115200 baud

### Required Libraries
- `ESP32Servo` - Hardware PWM timer management
- `Adafruit_SSD1306` - OLED framebuffer rendering
- `Adafruit_GFX` - Graphics primitives
- `WiFi.h`, `WebServer.h`, `DNSServer.h` - Network stack

## Architecture

### Directory Structure
- `SesameAdafruit/` - Modern Adafruit-based implementation (primary)
- `SesameOriginal/firmware/` - Original firmware variants for different boards
- `AdafruitServoExample/`, `AdafruitSSD1306Example/` - Reference implementations

### Key Files
- `SesameAdafruit/SesameAdafruit/SesameAdafruit.ino` - Main firmware (~865 lines)
- `SesameAdafruit/SesameAdafruit/Faces.h` - PROGMEM bitmap arrays for face expressions
- `SesameOriginal/firmware/debugging-firmware/sesame-motor-tester.ino` - Calibration utility

### Hardware Abstraction
Pin definitions are abstracted via `servoPins` array at sketch top. Modify these for different ESP32 variants:
```cpp
const int servoPins[8] = {15, 2, 23, 19, 4, 16, 17, 18};  // Distro Board
```

### Web API Endpoints
- `/` - Main controller UI
- `/cmd?go=[forward|backward|left|right]` - Movement commands
- `/cmd?pose=[rest|stand|wave|dance|...]` - Animation poses
- `/getSettings`, `/setSettings` - JSON configuration

## Key Patterns

### Non-Blocking Control
Use `pressingCheck(String cmd, int ms)` instead of `delay()` for interruptible animations. This polls web server handlers during motion frames.

### PWM Timer Allocation
All 4 hardware timers are reserved at startup. Adding additional PWM devices may cause network errors:
```cpp
ESP32PWM::allocateTimer(0);
ESP32PWM::allocateTimer(1);
ESP32PWM::allocateTimer(2);
ESP32PWM::allocateTimer(3);
```

### Memory Optimization
Face bitmaps (1024 bytes each) use `PROGMEM` to prevent SRAM exhaustion.

### Motor Brownout Prevention
`motorCurrentDelay` (default 20ms) staggers servo activation to prevent VCC rail collapse. Adjustable via web settings.

## Serial Commands (Motor Tester)
- `id,angle` - Set single servo (e.g., "0,90")
- `all,angle` - Set all servos
- `stop` - Disable all servos

## Face Asset Pipeline
1. Create 128x64 image using Kaomoji/Emojicombos + jsPaint
2. Convert with image2cpp (Horizontal scaling, Arduino Code output)
3. Add to `Faces.h` as `PROGMEM const unsigned char` array
