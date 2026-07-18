#include <Arduino.h>
#include "globals.h"
#include "config.h"
#include "helpers.h"
#include "sounds.h"
#include "button.h"
#include "display.h"
#include "network.h"
#include "extensions.h"

void setup() {
  Serial.begin(115200);
  delay(100);

  sound_init();

  Wire.begin(I2C_SDA, I2C_SCL);
  disp.begin();
  disp.setPowerSave(0);
  delay(100);

  rtc_began = rtc.begin(&Wire);

  button_setup();

  show_splash_screen();
  sound_startup();

  connect_to_wifi();
  sync_time_ntp();

  set_time_from_rtc();

  extensions_setup();

  time_t now;
  time(&now);
  localtime_r(&now, &timeinfo);
}

void loop() {
  time_t now;
  time(&now);
  localtime_r(&now, &timeinfo);

  unsigned long now_ms = millis();

  button_loop();
  extensions_loop();

  if (now_ms - last_screen_switch >= 6000) {
    if (current_screen == EXT_POMODORO_SCREEN) {
      if (now_ms - last_btn_activity_ms < 10000) {
        last_screen_switch = now_ms;
      } else {
        current_screen = (current_screen + 1) % extensions_screen_count();
        last_screen_switch = now_ms;
      }
    } else {
      current_screen = (current_screen + 1) % extensions_screen_count();
      last_screen_switch = now_ms;
    }
  }

  if (current_screen == 0) {
    update_gui();
  } else {
    extensions_render();
  }

  delay(100);
}
