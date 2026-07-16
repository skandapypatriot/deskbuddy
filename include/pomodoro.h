#pragma once
#include "globals.h"

#define BUZZER_ON()  digitalWrite(BUZZER_PIN, LOW)
#define BUZZER_OFF() digitalWrite(BUZZER_PIN, HIGH)

inline void buzzer_beep(int times, int on_ms = 150, int off_ms = 150) {
  for (int i = 0; i < times; i++) {
    BUZZER_ON();
    delay(on_ms);
    BUZZER_OFF();
    if (i < times - 1) delay(off_ms);
  }
}

inline void handle_button_and_pomo() {
  bool btn_pressed = (digitalRead(BTN_PIN) == LOW);

  if (btn_pressed && !btn_was_pressed) {
    btn_press_start = millis();
    btn_long_fired  = false;
    btn_was_pressed = true;
  }

  if (btn_pressed && btn_was_pressed && !btn_long_fired) {
    if (millis() - btn_press_start >= LONG_PRESS_MS) {
      btn_long_fired = true;

      current_screen     = 2;
      pomo_screen_active = true;

      if (pomo_done) {
        pomo_done          = false;
        pomo_running       = false;
        pomo_screen_active = false;
        current_screen     = 0;
        last_screen_switch = millis();
      } else if (pomo_running) {
        pomo_running       = false;
        pomo_done          = false;
        pomo_screen_active = false;
        current_screen     = 0;
        last_screen_switch = millis();
        BUZZER_OFF();
      } else {
        pomo_duration_ms = (unsigned long)POMO_PRESETS[pomo_preset_idx] * 60UL * 1000UL;
        pomo_start_ms    = millis();
        pomo_running     = true;
        pomo_done        = false;
        buzzer_beep(1, 80);
      }
    }
  }

  if (!btn_pressed && btn_was_pressed) {
    if (!btn_long_fired) {
      if (current_screen != 2) {
        current_screen = 2;
      } else if (!pomo_running && !pomo_done) {
        pomo_preset_idx = (pomo_preset_idx + 1) % POMO_NUM_PRESETS;
        buzzer_beep(1, 30);
      }
    }
    btn_was_pressed = false;
  }

  if (pomo_running) {
    unsigned long elapsed = millis() - pomo_start_ms;
    if (elapsed >= pomo_duration_ms) {
      pomo_running = false;
      pomo_done    = true;
      buzzer_beep(3, 300, 150);
    }
  }
}
