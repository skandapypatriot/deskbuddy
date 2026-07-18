#pragma once
#include <Arduino.h>
#include <Wire.h>
#include <U8g2lib.h>
#include <RTClib.h>
#include <WiFi.h>
#include <time.h>

#define I2C_SDA       8
#define I2C_SCL       9
#define BTN_PIN       4
#define BUZZER_PIN    10
#define LONG_PRESS_MS 800

extern U8G2_SSD1306_128X64_NONAME_1_HW_I2C disp;
extern RTC_DS3231 rtc;
extern bool     rtc_began;
extern struct tm timeinfo;
extern bool     wifi_connected;
extern int      current_screen;
extern unsigned long last_screen_switch;
extern unsigned long last_btn_activity_ms;

extern float weather_temp;
extern int   weather_humidity;
extern char  weather_desc[32];
extern char  weather_loc[32];
extern unsigned long last_weather_fetch;
extern int   weather_code;

extern int   forecastCodes[3];
extern float forecastHighs[3];
extern float forecastLows[3];
extern int   forecastRain[3];
extern char  forecastDays[3][4];

extern const uint8_t POMO_PRESETS[];
extern const uint8_t POMO_NUM_PRESETS;
extern uint8_t       pomo_preset_idx;
extern bool          pomo_running;
extern unsigned long pomo_start_ms;
extern unsigned long pomo_duration_ms;
extern bool          pomo_done;

extern bool          modem_sound_active;
