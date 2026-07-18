#pragma once
#include "globals.h"
#include "button.h"
#include "sounds.h"

#define EXT_POMODORO_SCREEN 2

// ── Pomodoro logic — consumes button.h events ──────────────────────────────

inline void ext_pomodoro_handle_button() {
  if (current_screen != EXT_POMODORO_SCREEN) return;

  // Long press: start / cancel / dismiss
  if (btn_long_event) {
    if (pomo_done) {
      sound_alarm_stop();
      pomo_done    = false;
      pomo_running = false;
    } else if (pomo_running) {
      sound_alarm_stop();
      sound_pomo_cancel();
      pomo_running = false;
      pomo_done    = false;
    } else {
      pomo_duration_ms = (unsigned long)POMO_PRESETS[pomo_preset_idx] * 60UL * 1000UL;
      pomo_start_ms    = millis();
      pomo_running     = true;
      pomo_done        = false;
      sound_pomo_start();
    }
  }

  // Short tap: cycle presets (only when idle)
  if (btn_tap_event) {
    if (!pomo_running && !pomo_done) {
      pomo_preset_idx = (pomo_preset_idx + 1) % POMO_NUM_PRESETS;
      sound_tap();
    }
  }

  // Timer tick
  if (pomo_running) {
    unsigned long elapsed = millis() - pomo_start_ms;
    if (elapsed >= pomo_duration_ms) {
      pomo_running = false;
      pomo_done    = true;
      sound_alarm_start();
    }
  }

  if (pomo_done) {
    sound_alarm_tick();
  }
}

// ── Render ─────────────────────────────────────────────────────────────────

inline void ext_pomodoro_render() {
  disp.setFont(u8g2_font_profont10_tr);
  const char* pomo_title = "POMODORO";
  int pt_w = disp.getStrWidth(pomo_title);
  disp.drawStr((128 - pt_w) / 2, 8, pomo_title);
  disp.drawHLine(0, 12, 128);

  if (pomo_done) {
    disp.setFont(u8g2_font_logisoso16_tr);
    const char* done_str = "DONE!";
    int dw = disp.getStrWidth(done_str);
    disp.drawStr((128 - dw) / 2, 40, done_str);
    disp.setFont(u8g2_font_profont11_tf);
    const char* hint = "Hold to reset";
    int hw = disp.getStrWidth(hint);
    disp.drawStr((128 - hw) / 2, 58, hint);

  } else if (pomo_running) {
    unsigned long elapsed = millis() - pomo_start_ms;
    unsigned long remaining_ms = (elapsed >= pomo_duration_ms) ? 0 : (pomo_duration_ms - elapsed);
    int rem_min = remaining_ms / 60000;
    int rem_sec = (remaining_ms % 60000) / 1000;

    char countdown_buf[8];
    snprintf(countdown_buf, sizeof(countdown_buf), "%02d:%02d", rem_min, rem_sec);
    disp.setFont(u8g2_font_logisoso24_tr);
    int cw = disp.getStrWidth(countdown_buf);
    disp.drawStr((128 - cw) / 2, 42, countdown_buf);

    disp.setFont(u8g2_font_profont11_tf);
    const char* run_hint = "Hold=Reset";
    int rh_w = disp.getStrWidth(run_hint);
    disp.drawStr((128 - rh_w) / 2, 53, run_hint);

    int bar_fill = (int)((128.0f * elapsed) / pomo_duration_ms);
    if (bar_fill > 128) bar_fill = 128;
    disp.drawFrame(0, 58, 128, 4);
    if (bar_fill > 0) disp.drawBox(0, 58, bar_fill, 4);

  } else {
    char dur_buf[16];
    snprintf(dur_buf, sizeof(dur_buf), "%d MIN", POMO_PRESETS[pomo_preset_idx]);
    disp.setFont(u8g2_font_logisoso24_tr);
    int dw = disp.getStrWidth(dur_buf);
    disp.drawStr((128 - dw) / 2, 46, dur_buf);

    disp.setFont(u8g2_font_profont11_tf);
    const char* idle_hint = "Tap+  Hold=Start";
    int iw = disp.getStrWidth(idle_hint);
    disp.drawStr((128 - iw) / 2, 62, idle_hint);
  }
}

// ── Extension interface ────────────────────────────────────────────────────

inline void ext_pomodoro_setup() {
}

inline void ext_pomodoro_loop() {
  ext_pomodoro_handle_button();
}
