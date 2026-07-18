#pragma once
#include "globals.h"
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

    disp.setFont(u8g2_font_profont12_tr);
    int title_width = disp.getStrWidth("DESK BUDDY");
    disp.drawStr((128 - title_width) / 2, 14, "DESK BUDDY");

    disp.drawHLine(10, 18, 108);

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

// ── Clock screen (screen 0, always present) ────────────────────────────────

inline void update_gui() {
  disp.firstPage();
  do {
    disp.setDrawColor(1);

    disp.setFont(u8g2_font_profont10_tr);

    char date_text_buf[50] = "";
    snprintf(date_text_buf, sizeof(date_text_buf), "%d%s %s, %s",
             timeinfo.tm_mday, get_ordinal(timeinfo.tm_mday), get_month_str(), get_day_str());
    int bufwidth = disp.getStrWidth(date_text_buf);
    int bufX = (128 - bufwidth) / 2;
    disp.drawStr(bufX, 8, date_text_buf);

    draw_wifi_icon(120, 1, WiFi.status() == WL_CONNECTED);

    disp.drawHLine(0, 12, 128);

    char timebuf[20] = "";
    snprintf(timebuf, sizeof(timebuf), "%02d:%02d", timeinfo.tm_hour, timeinfo.tm_min);
    disp.setFont(u8g2_font_logisoso24_tr);
    int timeWidth = disp.getStrWidth(timebuf);
    int timeX = (128 - timeWidth) / 2;
    disp.drawStr(timeX, 43, timebuf);

    char datebuf[25] = "";
    snprintf(datebuf, sizeof(datebuf), "%02d/%02d/%d", timeinfo.tm_mday, timeinfo.tm_mon + 1, timeinfo.tm_year + 1900);
    disp.setFont(u8g2_font_profont12_tr);
    int dateWidth = disp.getStrWidth(datebuf);
    int dateX = (128 - dateWidth) / 2;
    disp.drawStr(dateX, 58, datebuf);

  } while (disp.nextPage());
}
