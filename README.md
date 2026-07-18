# Desk Buddy

A modular ESP32-C3 desktop companion with clock, weather, and pomodoro timer on SSD1306 OLED. Features a plug-and-play extension system — just drop a file in and rebuild.

## Hardware

| Component | Connection |
|---|---|
| ESP32-C3 DevKitM-1 | Main MCU |
| SSD1306 128x64 OLED | I2C — SDA: Pin 8, SCL: Pin 9 |
| DS3231 RTC Module | I2C (shared bus) |
| Active Buzzer | Pin 10 (active-LOW) |
| Tactile Button | Pin 4 (INPUT_PULLUP, hardware interrupt) |

## Features

- **Clock** — Real-time clock with date, day, and WiFi icon
- **Weather** — Live conditions + 3-day forecast via WeatherAPI
- **Pomodoro** — Preset timer (5/10/15/20/25 min), progress bar, buzzer alarm
- **Modem Sound** — Dial-up handshake during WiFi connect (button to abort)
- **Drop-in Extensions** — Auto-detected from `include/extensions/` folder

## Building

```bash
# Install PlatformIO, then:
pio run              # Build
pio run -t upload    # Build + upload to COM port
```

## Extension System

Drop a `.h` file into `include/extensions/` and rebuild. The build system auto-detects it.

**Required interface:**
```cpp
#pragma once
#include "globals.h"

#define EXT_MYEXT_SCREEN 3  // unique screen number

inline void ext_myext_setup()  { /* called once at boot */ }
inline void ext_myext_loop()   { /* called every loop */ }
inline void ext_myext_render() { /* draw to disp (U8G2, 128x64) */ }
```

Broken files are automatically excluded with a build warning.

See [docs/WRITING_EXTENSIONS.md](docs/WRITING_EXTENSIONS.md) for the full guide.

## Project Structure

```
deskbuddy/
├── include/
│   ├── extensions/          # Drop-in extensions (auto-detected)
│   │   ├── ext_pomodoro.h
│   │   └── ext_weather.h
│   ├── button.h             # Hardware interrupt + button state machine
│   ├── config.h             # WiFi, NTP, WeatherAPI credentials
│   ├── display.h            # Clock screen + boot/splash utils
│   ├── extensions.h         # Includes auto-generated registry
│   ├── globals.h            # All extern declarations
│   ├── helpers.h            # Date string utilities
│   ├── icons.h              # Weather icon bitmaps
│   ├── network.h            # WiFi connect + NTP sync
│   └── sounds.h             # All buzzer sounds + modem handshake
├── scripts/
│   └── scan_extensions.py   # Pre-build: scans extensions folder
├── src/
│   ├── globals.cpp          # Variable definitions
│   └── main.cpp             # Entry point
├── docs/
│   └── WRITING_EXTENSIONS.md
├── goals.md
├── platformio.ini
└── README.md
```

## Configuration

Edit `include/config.h`:
```cpp
const WifiNetwork wifi_networks[] = {
    {"YourSSID", "YourPassword"},
};
const char* weather_api_key    = "your_weatherapi_key";
const char* weather_location   = "Bengaluru";
```

## License

MIT
