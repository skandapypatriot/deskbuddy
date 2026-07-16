# Desk Buddy - Project Evaluation & Roadmap

This document provides a comprehensive evaluation of the **Desk Buddy** project and outlines immediate fixes, medium-term feature additions, and long-term hardware/software goals.

---

## 1. Current Project Status

Desk Buddy is an ESP32-C3 and Arduino-based desktop companion project. It currently functions as a basic digital clock that displays the date, time, and day of the week on an OLED screen using a Real-Time Clock (RTC) module.

### Hardware Stack
*   **Microcontroller**: ESP32-C3 (ESP32-C3-DevKitM-1)
*   **Display**: SSD1306 128x64 OLED Display (connected via I2C)
*   **Timekeeping**: DS3231 Real-Time Clock (RTC) Module (connected via I2C)
*   **I2C Pins**: SDA is mapped to Pin 8; SCL is mapped to Pin 9

### Software Stack
*   **Framework**: Arduino Framework on PlatformIO
*   **Key Libraries** (defined in [platformio.ini](file:///C:/Users/Skanda/Pictures/Projects/deskbuddy/platformio.ini)):
    *   `adafruit/RTClib` (RTC communication)
    *   `olikraus/U8g2` (OLED graphics & text rendering)
    *   `bblanchon/ArduinoJson` (JSON parsing - not yet used in code)

---

## 2. Identified Issues & Code Quality Bugs

Upon evaluating the existing codebase, several issues and bugs have been identified:

1.  **Month Array Typo (Critical Logic Bug)**:
    In [src/main.cpp](file:///C:/Users/Skanda/Pictures/Projects/deskbuddy/src/main.cpp#L18) inside [get_month_str](file:///C:/Users/Skanda/Pictures/Projects/deskbuddy/src/main.cpp#L16):
    ```cpp
    char* months[] = {"","January", "February", "March", "April", "May", "June" "July", "August", "September", "October", "November", "December"};
    ```
    There is a missing comma between `"June"` and `"July"`. C++ automatically concatenates adjacent string literals, resulting in a single element `"JuneJuly"`. This makes the array size 12 instead of 13 and offsets all subsequent months (e.g., August becomes index 7 instead of 8, etc.).
2.  **Const Correctness Warnings (Compiler Warnings)**:
    The functions [get_month_str](file:///C:/Users/Skanda/Pictures/Projects/deskbuddy/src/main.cpp#L16) and [get_day_str](file:///C:/Users/Skanda/Pictures/Projects/deskbuddy/src/main.cpp#L21) return `char*` but point to string literals (which are `const char*` in C++). This produces compiler warnings:
    `warning: ISO C++ forbids converting a string constant to 'char*' [-Wwrite-strings]`
3.  **Hardcoded API Key (Security Risk)**:
    There is an OpenRouter API key commented out at the bottom of [src/main.cpp](file:///C:/Users/Skanda/Pictures/Projects/deskbuddy/src/main.cpp#L83). Keeping API keys in source control is a security risk. This should be moved to a private configuration header or build environment flag.
4.  **Utility File Placement**:
    [i2cscanner.cpp](file:///C:/Users/Skanda/Pictures/Projects/deskbuddy/i2cscanner.cpp) is placed in the project root directory. Since PlatformIO compiles everything in the `src` folder, this file is ignored during builds. While this prevents build conflicts with `main.cpp`, it is cleaner to configure utility files as secondary build environments in [platformio.ini](file:///C:/Users/Skanda/Pictures/Projects/deskbuddy/platformio.ini).

---

## 3. Project Roadmap & Goals

### Phase 1: Short-Term (Code Clean-up & Security)
- [x] **Fix Month Array & Warnings**: Correct the missing comma in the `months` array and update return types to `const char*`.
- [x] **Secure API Keys**: Create a `config.h` (or use PlatformIO build flags) to manage sensitive keys/credentials. Add `include/config.h` to [.gitignore](file:///C:/Users/Skanda/Pictures/Projects/deskbuddy/.gitignore).
- [ ] **Refactor I2C Scanner**: Move [i2cscanner.cpp](file:///C:/Users/Skanda/Pictures/Projects/deskbuddy/i2cscanner.cpp) or setup a multi-environment configuration in [platformio.ini](file:///C:/Users/Skanda/Pictures/Projects/deskbuddy/platformio.ini) to allow building the scanner when debugging hardware.

### Phase 2: Medium-Term (Network & API Integration)
- [x] **Wi-Fi Manager / Connection**: Implement Wi-Fi connectivity on startup so the ESP32-C3 can access online services.
- [x] **NTP Time Synchronization**: Sync the DS3231 RTC automatically with an NTP time server on startup (and periodically) so the clock never drifts and adjusts for daylight saving.

- [ ] **OpenRouter LLM Integration**:
    *   Implement an HTTP client to query OpenRouter APIs.
    *   Parse the response using `ArduinoJson` (already in dependencies).
    *   Display motivational quotes, weather summaries, or AI-generated reminders on the OLED screen.
- [ ] **OLED Burn-in Protection**: Implement a screensaver mode or auto-dimming feature to protect the SSD1306 display.

### Phase 3: Long-Term (Hardware Expansion & UI)
- [ ] **Physical Navigation**: Add a rotary encoder or physical buttons to navigate between different screens (e.g. Clock, Weather, Task List, AI Response).
- [ ] **Sensor Integration**: Add a temperature/humidity sensor (like a DHT22 or BME280) to show local desk environmental readings.
- [ ] **Audio Alerts**: Add a passive buzzer/speaker to sound alarms, timers, or subtle notifications.
- [ ] **3D Printed Enclosure**: Design and print a retro or modern enclosure to house the ESP32-C3, OLED, RTC, and future sensors on a desk.
