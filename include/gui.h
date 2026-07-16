#ifndef GUI_H
#define GUI_H

#include <U8g2lib.h>
#include "WiFi.h"


void draw_wifi_icon(int x, int y, bool connected) {
  if (connected) {
    // Draw 3-bar signal strength indicator
    disp.drawBox(x, y + 4, 1, 2);     // 1st bar (lowest)
    disp.drawBox(x + 2, y + 2, 1, 4); // 2nd bar
    disp.drawBox(x + 4, y, 1, 6);     // 3rd bar (highest)
  } else {
    // Draw a small 'x' to show disconnected
    disp.drawLine(x, y + 1, x + 4, y + 5);
    disp.drawLine(x, y + 5, x + 4, y + 1);
  }
}
void screen_one(){
// --- TOP BAR (only on Screen 1) ---
      disp.drawBox(0, 0, 128, 17);
      disp.setDrawColor(0);
      disp.setFont(u8g2_font_profont12_tr);
      
      char date_text_buf[50] = "";
      snprintf(date_text_buf, sizeof(date_text_buf), "%d%s %s, %s", now.day(), get_ordinal(now.day()), get_month_str(), get_day_str());
      int bufwidth = disp.getStrWidth(date_text_buf);
      int bufX = (128 - bufwidth) / 2;
      disp.drawStr(bufX, 12, date_text_buf);
      
      bool is_wifi_connected = (WiFi.status() == WL_CONNECTED);
      draw_wifi_icon(120, 5, is_wifi_connected);
      
      disp.setDrawColor(1);
      
      // SCREEN 1: Clock & Date
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
}

#endif 
