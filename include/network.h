#pragma once
// ═══════════════════════════════════════════════════════════════════════════
//  network.h  –  Wi-Fi, NTP, and Weather API networking
// ═══════════════════════════════════════════════════════════════════════════

#include <time.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "esp_wifi.h"
#include "globals.h"
#include "config.h"
#include "display.h"

// ── connect_to_wifi() ────────────────────────────────────────────────────────
//   Tries each network in wifi_networks[] in order.
inline void connect_to_wifi() {
  int num_networks = sizeof(wifi_networks) / sizeof(wifi_networks[0]);

  if (WiFi.getMode() != WIFI_STA) WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  WiFi.setSleep(WIFI_PS_NONE);
  esp_wifi_set_max_tx_power(WIFI_POWER_8_5dBm);

  for (int i = 0; i < num_networks; i++) {
    Serial.printf("Connecting to network SSID: %s\n", wifi_networks[i].ssid);
    WiFi.begin(wifi_networks[i].ssid, wifi_networks[i].password);

    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 20) {
      int progress = (attempts * 100) / 20;
      char status_buf[32];
      snprintf(status_buf, sizeof(status_buf), "Connecting (%d%%)", progress);
      show_boot_status(status_buf, wifi_networks[i].ssid, progress);

      delay(500);
      Serial.print(".");
      attempts++;
    }

    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("\nWiFi Connected successfully!");
      wifi_connected = true;
      show_boot_status("WiFi Connected!", wifi_networks[i].ssid, 100);
      delay(1000);
      return;
    }
    Serial.println("\nConnection failed.");
  }

  wifi_connected = false;
  show_boot_status("WiFi Offline", "Using RTC time", -1);
  delay(1500);
}

// ── sync_time_ntp() ──────────────────────────────────────────────────────────
//   Syncs the DS3231 RTC from an NTP server.
inline void sync_time_ntp() {
  if (!wifi_connected) return;

  show_boot_status("Syncing NTP...", "Contacting NTP server", 25);
  Serial.println("Synchronizing time with NTP server...");
  configTime(gmt_offset_sec, daylight_offset_sec, ntp_server);

  struct tm timeinfo;
  bool sync_success = false;
  for (int i = 0; i < 10; i++) {
    show_boot_status("Syncing NTP...", "Contacting NTP server", 25 + (i * 5));
    if (getLocalTime(&timeinfo, 1000)) {
      sync_success = true;
      break;
    }
    Serial.print(".");
  }

  if (sync_success) {
    Serial.println("NTP Sync Successful!");
    rtc.adjust(DateTime(timeinfo.tm_year + 1900, timeinfo.tm_mon + 1, timeinfo.tm_mday,
                         timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec));
    show_boot_status("NTP Sync Success!", "RTC Calibrated", 100);
    delay(1000);
  } else {
    Serial.println("Failed to obtain time from NTP.");
    show_boot_status("NTP Sync Failed", "Using RTC time", -1);
    delay(1500);
  }
}

// ── fetch_weather() ──────────────────────────────────────────────────────────
//   Fetches current + 3-day forecast from WeatherAPI.com.
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
