#pragma once
#include "globals.h"

// ── Hardware interrupt flag ────────────────────────────────────────────────

volatile bool _btn_isr_fired = false;

void IRAM_ATTR _btn_isr() {
  _btn_isr_fired = true;
}

// ── Button state ───────────────────────────────────────────────────────────

static bool          _btn_was_pressed  = false;
static unsigned long _btn_press_start  = 0;
static bool          _btn_long_fired   = false;

// One-shot event flags — cleared each loop by the consumer
bool btn_tap_event    = false;
bool btn_long_event   = false;

// ── Setup ──────────────────────────────────────────────────────────────────

inline void button_setup() {
  pinMode(BTN_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BTN_PIN), _btn_isr, FALLING);
}

// ── Call once per main-loop iteration ──────────────────────────────────────

inline void button_loop() {
  _btn_isr_fired = false;
  btn_tap_event  = false;
  btn_long_event = false;

  bool btn_pressed = (digitalRead(BTN_PIN) == LOW);

  // Press edge
  if (btn_pressed && !_btn_was_pressed) {
    _btn_press_start = millis();
    _btn_long_fired  = false;
    _btn_was_pressed = true;
  }

  // Long press detection
  if (btn_pressed && _btn_was_pressed && !_btn_long_fired) {
    if (millis() - _btn_press_start >= LONG_PRESS_MS) {
      _btn_long_fired  = true;
      btn_long_event   = true;
      last_btn_activity_ms = millis();
    }
  }

  // Release edge — short tap
  if (!btn_pressed && _btn_was_pressed) {
    if (!_btn_long_fired) {
      btn_tap_event = true;
      last_btn_activity_ms = millis();
    }
    _btn_was_pressed = false;
  }
}

inline bool button_is_pressed() {
  return digitalRead(BTN_PIN) == LOW;
}
