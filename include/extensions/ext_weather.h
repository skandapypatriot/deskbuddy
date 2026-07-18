#pragma once
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "globals.h"
#include "config.h"
#include "icons.h"

#define EXT_WEATHER_SCREEN 1

// ── Fetch weather data ─────────────────────────────────────────────────────

inline void fetch_weather() {
  if (WiFi.status() != WL_CONNECTED) return;

  WiFiClient client;
  HTTPClient http;

  char url[256];
  snprintf(url, sizeof(url),
           "http://api.weatherapi.com/v1/forecast.json?key=%s&q=%s&days=3&aqi=no",
           weather_api_key, weather_location);

  Serial.printf("Fetching weather: %s\n", url);
  if (http.begin(client, url)) {
    int httpCode = http.GET();
    if (httpCode == 200) {
      String payload = http.getString();
      JsonDocument doc;
      DeserializationError error = deserializeJson(doc, payload);

      if (!error) {
        weather_temp     = doc["current"]["temp_c"]        | 0.0;
        weather_humidity = doc["current"]["humidity"]       | 0;
        weather_code     = doc["current"]["condition"]["code"] | 1000;

        const char* desc = doc["current"]["condition"]["text"] | "N/A";
        const char* loc  = doc["location"]["name"]             | "N/A";

        strncpy(weather_desc, desc, sizeof(weather_desc) - 1);
        weather_desc[sizeof(weather_desc) - 1] = '\0';
        strncpy(weather_loc, loc, sizeof(weather_loc) - 1);
        weather_loc[sizeof(weather_loc) - 1] = '\0';

        JsonArray forecastDayArr = doc["forecast"]["forecastday"].as<JsonArray>();
        int i = 0;
        for (JsonObject day : forecastDayArr) {
          if (i >= 3) break;
          forecastCodes[i] = day["day"]["condition"]["code"]       | 1000;
          forecastHighs[i] = day["day"]["maxtemp_c"]              | 0.0;
          forecastLows[i]  = day["day"]["mintemp_c"]              | 0.0;
          forecastRain[i]  = day["day"]["daily_chance_of_rain"]   | 0;

          const char* dateStr = day["date"] | "";
          if (strlen(dateStr) == 10) {
            struct tm tmDate = {};
            int y, m, d;
            sscanf(dateStr, "%d-%d-%d", &y, &m, &d);
            tmDate.tm_year = y - 1900;
            tmDate.tm_mon  = m - 1;
            tmDate.tm_mday = d;
            mktime(&tmDate);
            const char* dayNames[] = {"SUN","MON","TUE","WED","THU","FRI","SAT"};
            strncpy(forecastDays[i], dayNames[tmDate.tm_wday], 3);
            forecastDays[i][3] = '\0';
          }
          i++;
        }

        last_weather_fetch = millis();
        Serial.printf("Weather synced: Temp=%.1f, Humidity=%d, Code=%d, Loc=%s\n",
                      weather_temp, weather_humidity, weather_code, weather_loc);
        Serial.printf("Forecast: [%s %d/%d] [%s %d/%d] [%s %d/%d]\n",
          forecastDays[0], (int)forecastHighs[0], (int)forecastLows[0],
          forecastDays[1], (int)forecastHighs[1], (int)forecastLows[1],
          forecastDays[2], (int)forecastHighs[2], (int)forecastLows[2]);
      } else {
        Serial.printf("Failed to parse weather JSON: %s\n", error.c_str());
      }
    } else {
      Serial.printf("Weather HTTP GET failed: %d\n", httpCode);
    }
    http.end();
  }
}

// ── Render ─────────────────────────────────────────────────────────────────

inline void ext_weather_render() {
  disp.setFont(u8g2_font_profont11_tf);
  for (int i = 0; i < 3; i++) {
    int x_pos = 2 + (i * 42);
    disp.drawStr(x_pos + 7, 8, forecastDays[i]);
    disp.drawXBMP(x_pos + 5, 10, 16, 16, get_weather_icon_16(forecastCodes[i]));
    disp.setFont(u8g2_font_4x6_tf);
    char hlBuf[12];
    snprintf(hlBuf, sizeof(hlBuf), "%.0f/%.0f", forecastHighs[i], forecastLows[i]);
    disp.drawStr(x_pos + 3, 33, hlBuf);
    char rainBuf[8];
    snprintf(rainBuf, sizeof(rainBuf), "%d%%", forecastRain[i]);
    disp.drawStr(x_pos + 7, 40, rainBuf);
    disp.setFont(u8g2_font_profont11_tf);
  }

  disp.drawHLine(0, 42, 128);

  disp.drawXBMP(2, 46, 16, 16, get_weather_icon_16(weather_code));
  disp.setFont(u8g2_font_profont11_tf);
  char todayBuf[32];
  snprintf(todayBuf, sizeof(todayBuf), "%.1fC %s", weather_temp, weather_loc);
  disp.drawStr(22, 55, todayBuf);
  disp.setFont(u8g2_font_4x6_tf);
  disp.drawStr(22, 62, weather_desc);
}

// ── Extension interface ────────────────────────────────────────────────────

inline void ext_weather_setup() {
  if (wifi_connected) fetch_weather();
}

inline void ext_weather_loop() {
  unsigned long now_ms = millis();
  if (wifi_connected && (last_weather_fetch == 0 || now_ms - last_weather_fetch >= 600000)) {
    fetch_weather();
  }
}
