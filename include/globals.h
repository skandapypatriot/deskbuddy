#pragma once
#include <Arduino.h>
#include <Wire.h>
#include <U8g2lib.h>
#include <RTClib.h>
#include <WiFi.h>

#define I2C_SDA 8
#define I2C_SCL 9

extern U8G2_SSD1306_128X64_NONAME_1_HW_I2C disp;
extern RTC_DS3231 rtc;
extern bool     rtc_began;
extern DateTime now;
extern bool     wifi_connected;
extern int      current_screen;
extern unsigned long last_screen_switch;

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
