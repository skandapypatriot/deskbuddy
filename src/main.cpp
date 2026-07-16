#include <Arduino.h>
#include "globals.h"
#include "config.h"
#include "helpers.h"
#include "icons.h"
#include "display.h"
#include "network.h"

void setup() {
  Serial.begin(115200);
  delay(100);

  Wire.begin(I2C_SDA, I2C_SCL);
  disp.begin();
  disp.setPowerSave(0);
  delay(100);

  rtc_began = rtc.begin(&Wire);

  show_splash_screen();

  connect_to_wifi();
  sync_time_ntp();

  if (wifi_connected) {
    fetch_weather();
  }

  now = rtc.now();
  if (now.unixtime() < DateTime(__DATE__, __TIME__).unixtime()) {
    rtc.adjust(DateTime(__DATE__, __TIME__));
  }
}

void loop() {
  now = rtc.now();

  unsigned long now_ms = millis();

  if (now_ms - last_screen_switch >= 6000) {
    current_screen = (current_screen == 0) ? 1 : 0;
    last_screen_switch = now_ms;
  }

  if (wifi_connected && (last_weather_fetch == 0 || now_ms - last_weather_fetch >= 600000)) {
    fetch_weather();
  }

  update_gui();
  delay(100);
}
