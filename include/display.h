#pragma once
#include "globals.h"
#include "icons.h"
#include "helpers.h"

#if __has_include("splash_icon.h")
  #include "splash_icon.h"
#endif

inline void draw_wifi_icon(int x, int y, bool connected) {
  if (connected) {
    disp.drawBox(x, y + 4, 1, 2);
    disp.drawBox(x + 2, y + 2, 1, 4);
    disp.drawBox(x + 4, y, 1, 6);
  } else {
    disp.drawLine(x, y + 1, x + 4, y + 5);
    disp.drawLine(x, y + 5, x + 4, y + 1);
  }
}

inline void show_splash_screen() {
  disp.firstPage();
  do {
#ifdef SPLASH_XBM
    disp.drawFrame(0, 0, 128, 64);
    disp.drawFrame(3, 3, 122, 58);
    disp.drawXBMP(64 - SPLASH_XBM_WIDTH / 2, 28 - SPLASH_XBM_HEIGHT / 2,
                  SPLASH_XBM_WIDTH, SPLASH_XBM_HEIGHT, SPLASH_XBM);
    disp.setFont(u8g2_font_profont12_tr);
    const char* title = "DESK BUDDY v1.0";
    int w = disp.getStrWidth(title);
    disp.drawStr((128 - w) / 2, 54, title);
#else
    disp.drawFrame(0, 0, 128, 64);
    disp.drawFrame(3, 3, 122, 58);
    disp.drawCircle(64, 26, 18);
    disp.drawCircle(64, 26, 17);
    disp.drawLine(64, 26, 64, 14);
    disp.drawLine(64, 26, 76, 26);
    disp.drawPixel(64, 8);
    disp.drawPixel(82, 26);
    disp.drawPixel(64, 44);
    disp.drawPixel(46, 26);
    disp.setFont(u8g2_font_profont12_tr);
    const char* title = "DESK BUDDY v1.0";
    int w = disp.getStrWidth(title);
    disp.drawStr((128 - w) / 2, 58, title);
#endif
  } while (disp.nextPage());
  delay(2000);
}

inline void show_boot_status(const char* status_msg, const char* sub_msg = "", int progress = -1) {
  disp.firstPage();
  do {
    disp.drawFrame(2, 2, 124, 60);

    disp.setDrawColor(1);
    disp.drawBox(4, 4, 120, 13);
    disp.setDrawColor(0);
    disp.setFont(u8g2_font_profont12_tr);
    int title_width = disp.getStrWidth("DESK BUDDY");
    disp.drawStr((128 - title_width) / 2, 14, "DESK BUDDY");
    disp.setDrawColor(1);

    disp.setFont(u8g2_font_profont12_tr);
    int msg_width = disp.getStrWidth(status_msg);
    disp.drawStr((128 - msg_width) / 2, 34, status_msg);

    if (strlen(sub_msg) > 0) {
      int sub_width = disp.getStrWidth(sub_msg);
      disp.drawStr((128 - sub_width) / 2, 46, sub_msg);
    }

    if (progress >= 0) {
      disp.drawFrame(14, 50, 100, 6);
      int fill_width = (progress * 96) / 100;
      if (fill_width > 0) {
        disp.drawBox(16, 52, fill_width, 2);
      }
    }
  } while (disp.nextPage());
}

inline void update_gui() {
  disp.firstPage();
  do {
    disp.setDrawColor(1);

    if (current_screen == 0) {
      disp.drawBox(0, 0, 128, 17);
      disp.setDrawColor(0);
      disp.setFont(u8g2_font_profont12_tr);

      char date_text_buf[50] = "";
      snprintf(date_text_buf, sizeof(date_text_buf), "%d%s %s, %s",
               now.day(), get_ordinal(now.day()), get_month_str(), get_day_str());
      int bufwidth = disp.getStrWidth(date_text_buf);
      int bufX = (128 - bufwidth) / 2;
      disp.drawStr(bufX, 12, date_text_buf);

      draw_wifi_icon(120, 5, WiFi.status() == WL_CONNECTED);

      disp.setDrawColor(1);

      char timebuf[20] = "";
      snprintf(timebuf, sizeof(timebuf), "%02i:%02i", now.hour(), now.minute());
      disp.setFont(u8g2_font_logisoso24_tr);
      int timeWidth = disp.getStrWidth(timebuf);
      int timeX = (128 - timeWidth) / 2;
      disp.drawStr(timeX, 47, timebuf);

      char datebuf[25] = "";
      snprintf(datebuf, sizeof(datebuf), "%02i/%02i/%i", now.day(), now.month(), now.year());
      disp.setFont(u8g2_font_profont12_tr);
      int dateWidth = disp.getStrWidth(datebuf);
      int dateX = (128 - dateWidth) / 2;
      disp.drawStr(dateX, 62, datebuf);

    } else if (current_screen == 1) {
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
  } while (disp.nextPage());
}
