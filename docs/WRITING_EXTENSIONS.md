# Writing Extensions for Desk Buddy

Desk Buddy uses a plug-and-play extension system. Extensions are **auto-detected** from the `include/extensions/` folder at build time. Just drop a file in and rebuild.

---

## Quick Start

### To add an extension:

1. **Create** `include/extensions/ext_<name>.h` using the template below
2. **Rebuild** — that's it. The build system detects it automatically.

### To remove an extension:

Delete the file from `include/extensions/` and rebuild.

### If an extension is broken:

The build system prints a `SKIP` warning and excludes it. Everything else still works.

---

## Extension Template

```cpp
#pragma once
#include "globals.h"
// Add other includes as needed (sounds.h, icons.h, config.h, etc.)

#define EXT_<NAME>_SCREEN N   // screen index: 1, 2, 3... (0 = clock, reserved)

// ── Called once after WiFi/NTP/splash ──
inline void ext_<name>_setup() {
  // e.g. fetch data, init hardware
}

// ── Called every loop iteration ──
inline void ext_<name>_loop() {
  // e.g. periodic data fetch, button handling, state machines
}

// ── Called when this screen is active ──
inline void ext_<name>_render() {
  // Draw to the OLED using the `disp` object (U8G2, 128x64)
  // Use disp.firstPage()/disp.nextPage() for buffer-friendly rendering
}
```

**Naming convention**: filename `ext_<name>.h` must match function names `ext_<name>_setup()`, `ext_<name>_loop()`, `ext_<name>_render()`. The scanner validates this — mismatched names are skipped.

---

## Required Interface

The build scanner (`scripts/scan_extensions.py`) checks each file for:

| Element | Pattern | Example |
|---|---|---|
| Screen define | `#define EXT_<NAME>_SCREEN <N>` | `#define EXT_SENSORS_SCREEN 3` |
| Setup function | `inline void ext_<name>_setup()` | `inline void ext_sensors_setup()` |
| Loop function | `inline void ext_<name>_loop()` | `inline void ext_sensors_loop()` |
| Render function | `inline void ext_<name>_render()` | `inline void ext_sensors_render()` |

If any are missing, the file is skipped with a warning.

---

## Screen Numbering

| Screen | Index | Owner |
|---|---|---|
| Clock | 0 | Always present (built into `display.h`) |
| Weather | 1 | `ext_weather.h` |
| Pomodoro | 2 | `ext_pomodoro.h` |
| Your extension | 3+ | Assign the next available number |

Screen numbers must be **unique** across all extensions.

---

## Available Globals

| Variable | Type | Description |
|---|---|---|
| `disp` | `U8G2_SSD1306...` | OLED display object (128x64) |
| `timeinfo` | `struct tm` | Current time (from ESP32 internal clock) |
| `wifi_connected` | `bool` | WiFi connection status |
| `current_screen` | `int` | Active screen index |
| `last_screen_switch` | `unsigned long` | millis() of last auto-rotate |
| `last_btn_activity_ms` | `unsigned long` | millis() of last button press |

### Button Events (from `button.h`)

Include `button.h` to use these one-shot flags (cleared each loop):

| Flag | Type | Description |
|---|---|---|
| `btn_tap_event` | `bool` | True for one loop on short tap |
| `btn_long_event` | `bool` | True for one loop on long press |
| `button_is_pressed()` | `bool` | Current button state |

---

## Available Utilities

- **`sounds.h`**: `sound_tone(freq, ms)`, `sound_tap()`, `sound_startup()`, `sound_alarm_start/tick/stop()`, `modem_connect_start/tick/stop()`
- **`display.h`**: `show_boot_status()`, `draw_wifi_icon()`, `show_splash_screen()`
- **`helpers.h`**: `get_ordinal()`, `get_month_str()`, `get_day_str()`
- **`icons.h`**: `get_weather_icon_16()`, `get_weather_icon_32()`
- **`button.h`**: `button_setup()`, `button_loop()`, `button_is_pressed()`

---

## Example: Sensor Extension

```cpp
// include/extensions/ext_sensors.h
#pragma once
#include "globals.h"

#define EXT_SENSORS_SCREEN 3

inline void ext_sensors_setup() {
  // init sensor pins
}

inline void ext_sensors_loop() {
  // read sensor every N seconds
}

inline void ext_sensors_render() {
  disp.setFont(u8g2_font_profont12_tr);
  disp.drawStr(10, 30, "Temp: 24.5C");
  disp.drawStr(10, 45, "Humidity: 60%");
}
```

Drop it in `include/extensions/` and rebuild. Done.
