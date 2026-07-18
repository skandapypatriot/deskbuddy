# Desk Buddy - Project Evaluation & Roadmap

This document provides a comprehensive evaluation of the **Desk Buddy** project and outlines immediate fixes, medium-term feature additions, and long-term hardware/software goals.

---

## 1. Current Project Status

Desk Buddy is an ESP32-C3 and Arduino-based desktop companion project. It functions as a digital clock that displays the date, time, and day of the week on an OLED screen using a Real-Time Clock (RTC) module. It also features live weather, a forecast screen, and a built-in Pomodoro timer with buzzer and button navigation.

### Hardware Stack
*   **Microcontroller**: ESP32-C3 (ESP32-C3-DevKitM-1)
*   **Display**: SSD1306 128x64 OLED Display (I2C: SDA=Pin 8, SCL=Pin 9)
*   **Timekeeping**: DS3231 Real-Time Clock (RTC) Module
*   **Buzzer**: Active buzzer on Pin 10 (active-LOW)
*   **Button**: Tactile button on Pin 4 (active-LOW, internal pull-up, hardware interrupt)

### Software Stack
*   **Framework**: Arduino Framework on PlatformIO
*   **Key Libraries**:
    *   `adafruit/RTClib` (RTC communication)
    *   `olikraus/U8g2` (OLED graphics & text rendering)
    *   `bblanchon/ArduinoJson` (Weather API JSON parsing)

### Architecture
Code is split across modular headers in `include/` with shared state defined in `src/globals.cpp`:
*   `config.h` — Wi-Fi credentials, NTP config, WeatherAPI key
*   `globals.h` / `src/globals.cpp` — `extern` declarations and definitions for all shared state
*   `button.h` — Hardware interrupt, button state machine (tap/long-press), available to all extensions
*   `helpers.h` — Date string utilities (`get_ordinal`, `get_month_str`, `get_day_str`)
*   `icons.h` — Weather icon bitmaps (32x32, 16x16) and lookup functions
*   `sounds.h` — All sound effects (startup, wifi, tap, pomo, alarm, dial-up modem)
*   `display.h` — OLED rendering (splash, boot status, clock screen)
*   `network.h` — Wi-Fi connect (with modem sound), NTP sync, RTC-to-ESP32 time sync
*   `extensions.h` → `extensions_generated.h` — Auto-generated plug-and-play extension registry
*   `include/extensions/` — Drop-in extension folder (auto-detected at build time)
*   `scripts/scan_extensions.py` — Pre-build script that scans extensions folder
*   `src/main.cpp` — `setup()` + `loop()` entry point

### Extension System
Extensions are auto-detected from `include/extensions/`. Drop a file in, rebuild, it works. Broken files are automatically excluded with a build warning.

**Extension contract** (each file must have):
*   `#define EXT_<NAME>_SCREEN <N>` — unique screen number
*   `ext_<name>_setup()` — called once at boot
*   `ext_<name>_loop()` — called every loop iteration
*   `ext_<name>_render()` — called when screen is active

### Current Build Stats
*   RAM: ~12%
*   Flash: ~64%

### Working Features
*   Real-time clock display with date, time, and day of week
*   Auto-rotating screens (clock, weather, pomodoro) every 6 seconds
*   Wi-Fi connectivity with NTP time sync
*   Live weather display (current conditions + 3-day forecast) via WeatherAPI
*   Pomodoro timer with preset durations, buzzer alerts, and progress bar
*   Single-button navigation: tap to cycle presets, hold to start/cancel/dismiss
*   Pomodoro screen locks for 10 seconds after button press (prevents auto-rotate)
*   Boot splash screen and status messages
*   Wi-Fi connection indicator on clock screen
*   Hardware interrupt button handling (via `button.h`)
*   ESP32 internal time set from RTC via `settimeofday()` — zero I2C traffic per loop
*   Dial-up modem handshake sound during WiFi connection (button-stoppable)
*   Drop-in extension system with auto-detection and graceful failure

---

## 2. Project Roadmap & Goals

### Phase 1: Short-Term (Code Clean-up & Security) — DONE
- [x] **Fix Month Array & Warnings**: Correct the missing comma in the `months` array and update return types to `const char*`.
- [x] **Secure API Keys**: Created `config.h` for sensitive credentials, added to `.gitignore`.
- [x] **Modularize Codebase**: Split monolithic `main.cpp` into header files with `extern` declarations and `globals.cpp` for definitions.
- [x] **Fix Screen Cycling**: Auto-rotation now cycles through all 3 screens (clock, weather, pomodoro).
- [x] **Fix Pomodoro Screen Layout**: Resolved collision between countdown, progress bar, and hint text.

### Phase 2: Medium-Term (Network & API Integration) — DONE
- [x] **Wi-Fi Manager / Connection**: Wi-Fi connectivity on startup.
- [x] **NTP Time Sync**: DS3231 RTC synced with NTP on startup.
- [x] **Weather API Integration**: Live weather and 3-day forecast via WeatherAPI with JSON parsing.
- [x] **RTC Time Handling**: Read RTC once at boot, set ESP32 internal clock via `settimeofday()`. No I2C per loop.
- [x] **Drop-in Extension System**: Auto-detect extensions from `include/extensions/` folder. No code editing needed.
- [x] **Button Library**: Standalone `button.h` with hardware interrupt, available to all extensions.
- [x] **Modem Sounds**: Dial-up handshake sound during WiFi connect, stoppable with button.

### Phase 3: Long-Term (Hardware Expansion & UI)
- [x] **Physical Navigation**: Single-button navigation for screen selection and pomodoro control.
- [x] **Audio Alerts**: Active buzzer for pomodoro start, completion, and preset cycling.
- [ ] **OLED Burn-in Protection**: Implement screensaver mode or auto-dimming to protect the SSD1306 display.
- [ ] **Sensor Integration**: Add a temperature/humidity sensor (DHT22 or BME280) for local desk environmental readings.
- [ ] **3D Printed Enclosure**: Design and print a retro or modern enclosure to house the ESP32-C3, OLED, RTC, buzzer, and button.
- [ ] **Multiple Pomodoro Profiles**: Allow configuring work/break durations from the device.
- [ ] **OTA Updates**: Implement over-the-air firmware updates via Wi-Fi.
- [ ] **OpenRouter LLM Integration**: Query OpenRouter APIs for motivational quotes, weather summaries, or AI-generated reminders on OLED.
