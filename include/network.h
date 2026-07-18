#pragma once
#include <time.h>
#include "esp_wifi.h"
#include "globals.h"
#include "config.h"
#include "display.h"
#include "sounds.h"
#include "button.h"

// ── connect_to_wifi() with modem sound ─────────────────────────────────────

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

    modem_connect_start();

    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 20) {
      int progress = (attempts * 100) / 20;
      char status_buf[32];
      snprintf(status_buf, sizeof(status_buf), "Connecting (%d%%)", progress);
      show_boot_status(status_buf, wifi_networks[i].ssid, progress);

      modem_connect_tick();

      if (button_is_pressed()) {
        modem_connect_stop();
        Serial.println("\nConnection aborted by user.");
        goto wifi_done;
      }

      delay(500);
      Serial.print(".");
      attempts++;
    }

    modem_connect_stop();

    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("\nWiFi Connected successfully!");
      wifi_connected = true;
      show_boot_status("WiFi Connected!", wifi_networks[i].ssid, 100);
      sound_wifi_connected();
      delay(1000);
      return;
    }
    Serial.println("\nConnection failed.");
  }

  wifi_connected = false;
  show_boot_status("WiFi Offline", "Using RTC time", -1);
  sound_wifi_failed();
  delay(1500);

wifi_done:
  modem_connect_stop();
}

// ── Set ESP32 internal time from RTC ───────────────────────────────────────

inline void set_time_from_rtc() {
  if (!rtc_began) return;

  DateTime rtc_now = rtc.now();
  struct timeval tv;
  tv.tv_sec  = rtc_now.unixtime();
  tv.tv_usec = 0;
  settimeofday(&tv, NULL);

  Serial.printf("ESP32 time set from RTC: %04d-%02d-%02d %02d:%02d:%02d\n",
                rtc_now.year(), rtc_now.month(), rtc_now.day(),
                rtc_now.hour(), rtc_now.minute(), rtc_now.second());
}

// ── sync_time_ntp() ────────────────────────────────────────────────────────

inline void sync_time_ntp() {
  if (!wifi_connected) return;

  show_boot_status("Syncing NTP...", "Contacting NTP server", 25);
  Serial.println("Synchronizing time with NTP server...");
  configTime(gmt_offset_sec, daylight_offset_sec, ntp_server);

  struct tm ntp_time;
  bool sync_success = false;
  for (int i = 0; i < 10; i++) {
    show_boot_status("Syncing NTP...", "Contacting NTP server", 25 + (i * 5));
    if (getLocalTime(&ntp_time, 1000)) {
      sync_success = true;
      break;
    }
    Serial.print(".");
  }

  if (sync_success) {
    Serial.println("NTP Sync Successful!");
    rtc.adjust(DateTime(ntp_time.tm_year + 1900, ntp_time.tm_mon + 1, ntp_time.tm_mday,
                         ntp_time.tm_hour, ntp_time.tm_min, ntp_time.tm_sec));
    show_boot_status("NTP Sync Success!", "RTC Calibrated", 100);
    delay(1000);
  } else {
    Serial.println("Failed to obtain time from NTP.");
    show_boot_status("NTP Sync Failed", "Using RTC time", -1);
    sound_wifi_failed();
    delay(1500);
  }
}
