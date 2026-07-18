#pragma once
#include "globals.h"

#define BUZZER_CHANNEL 0

// ── Hardware PWM init ──────────────────────────────────────────────────────

inline void sound_init() {
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, HIGH);
  ledcSetup(BUZZER_CHANNEL, 1000, 8);
  ledcAttachPin(BUZZER_PIN, BUZZER_CHANNEL);
  ledcWriteTone(BUZZER_CHANNEL, 0);
}

// ── primitives ─────────────────────────────────────────────────────────────

inline void sound_tone(uint32_t freq, unsigned long ms) {
  ledcWriteTone(BUZZER_CHANNEL, freq);
  delay(ms);
}

inline void sound_stop() {
  ledcWriteTone(BUZZER_CHANNEL, 0);
  digitalWrite(BUZZER_PIN, HIGH);
}

inline void sound_rest(unsigned long ms) {
  ledcWriteTone(BUZZER_CHANNEL, 0);
  delay(ms);
}

// ── startup: soft melodic chime ────────────────────────────────────────────

inline void sound_startup() {
  sound_tone(523, 120);
  sound_rest(50);
  sound_tone(659, 120);
  sound_rest(50);
  sound_tone(784, 120);
  sound_rest(50);
  sound_tone(1047, 220);
  sound_rest(80);
  sound_tone(784, 120);
  sound_rest(50);
  sound_tone(1047, 350);
  sound_stop();
}

// ── wifi connected: soft ascending ─────────────────────────────────────────

inline void sound_wifi_connected() {
  sound_tone(523, 100);
  sound_rest(40);
  sound_tone(659, 100);
  sound_rest(40);
  sound_tone(784, 100);
  sound_rest(40);
  sound_tone(1047, 200);
  sound_stop();
}

// ── wifi failed: soft descending ───────────────────────────────────────────

inline void sound_wifi_failed() {
  sound_tone(698, 160);
  sound_rest(60);
  sound_tone(622, 160);
  sound_rest(60);
  sound_tone(523, 160);
  sound_rest(60);
  sound_tone(440, 260);
  sound_stop();
}

// ── tap: gentle tick ───────────────────────────────────────────────────────

inline void sound_tap() {
  sound_tone(1200, 20);
  sound_rest(10);
  sound_tone(900, 15);
  sound_stop();
}

// ── pomo start ─────────────────────────────────────────────────────────────

inline void sound_pomo_start() {
  sound_tone(440, 80);
  sound_rest(40);
  sound_tone(554, 80);
  sound_rest(40);
  sound_tone(659, 80);
  sound_rest(40);
  sound_tone(880, 200);
  sound_stop();
}

// ── pomo cancel ────────────────────────────────────────────────────────────

inline void sound_pomo_cancel() {
  sound_tone(784, 100);
  sound_rest(40);
  sound_tone(659, 100);
  sound_rest(40);
  sound_tone(523, 100);
  sound_rest(40);
  sound_tone(440, 180);
  sound_stop();
}

// ── pomo alarm: non-blocking soft siren ────────────────────────────────────

static unsigned long _alarm_last_switch = 0;
static bool          _alarm_high        = true;

inline void sound_alarm_start() {
  _alarm_last_switch = millis();
  _alarm_high        = true;
  ledcWriteTone(BUZZER_CHANNEL, 880);
}

inline void sound_alarm_tick() {
  unsigned long t = millis();
  if (t - _alarm_last_switch >= 500) {
    _alarm_high = !_alarm_high;
    ledcWriteTone(BUZZER_CHANNEL, _alarm_high ? 880 : 784);
    _alarm_last_switch = t;
  }
}

inline void sound_alarm_stop() {
  ledcWriteTone(BUZZER_CHANNEL, 0);
  digitalWrite(BUZZER_PIN, HIGH);
}

// ═══════════════════════════════════════════════════════════════════════════
//  Dial-up modem handshake — non-blocking, button-stoppable
// ═══════════════════════════════════════════════════════════════════════════

enum ModemPhase : uint8_t {
  MODEM_DIAL_TONE,
  MODEM_DTMF,
  MODEM_DTMF_GAP,
  MODEM_CARRIER,
  MODEM_STATIC_BURST,
  MODEM_STATIC_GAP,
  MODEM_HANDSHAKE,
  MODEM_DONE
};

static ModemPhase    _modem_phase       = MODEM_DIAL_TONE;
static unsigned long _modem_phase_start = 0;
static int           _modem_dtmf_idx    = 0;

// Classic DTMF digit frequencies (row, col)
static const uint32_t _dtmf_freqs[8][2] = {
  {941, 1336}, {941, 1209}, {941, 1477}, {941, 1633},
  {852, 1336}, {852, 1209}, {852, 1477}, {852, 1633}
};

inline void modem_connect_start() {
  _modem_phase       = MODEM_DIAL_TONE;
  _modem_phase_start = millis();
  _modem_dtmf_idx    = 0;
  modem_sound_active = true;
  ledcWriteTone(BUZZER_CHANNEL, 350);
}

inline void modem_connect_stop() {
  ledcWriteTone(BUZZER_CHANNEL, 0);
  digitalWrite(BUZZER_PIN, HIGH);
  modem_sound_active = false;
}

inline void modem_connect_tick() {
  if (!modem_sound_active) return;

  unsigned long elapsed = millis() - _modem_phase_start;

  switch (_modem_phase) {

    case MODEM_DIAL_TONE:
      // Steady 350 Hz dial tone for 600ms
      if (elapsed >= 600) {
        _modem_phase = MODEM_DTMF;
        _modem_phase_start = millis();
        _modem_dtmf_idx = 0;
      }
      break;

    case MODEM_DTMF: {
      // 8 rapid DTMF digits, 60ms each with 30ms gaps
      int cycle = elapsed % 90;
      if (cycle < 60) {
        int d = _modem_dtmf_idx % 8;
        ledcWriteTone(BUZZER_CHANNEL, _dtmf_freqs[d][0] + _dtmf_freqs[d][1]);
      } else {
        ledcWriteTone(BUZZER_CHANNEL, 0);
      }
      if (elapsed >= 720) {
        _modem_phase = MODEM_DTMF_GAP;
        _modem_phase_start = millis();
        ledcWriteTone(BUZZER_CHANNEL, 0);
      }
      break;
    }

    case MODEM_DTMF_GAP:
      if (elapsed >= 200) {
        _modem_phase = MODEM_CARRIER;
        _modem_phase_start = millis();
      }
      break;

    case MODEM_CARRIER: {
      // V.21 carrier: alternate 1080/1750 Hz every 80ms
      int slot = (elapsed / 80) % 2;
      ledcWriteTone(BUZZER_CHANNEL, slot ? 1750 : 1080);
      if (elapsed >= 1200) {
        _modem_phase = MODEM_STATIC_BURST;
        _modem_phase_start = millis();
      }
      break;
    }

    case MODEM_STATIC_BURST: {
      // Pseudo-static: rapid random-ish frequency shifts
      uint32_t f = 800 + ((elapsed * 7) % 1400);
      ledcWriteTone(BUZZER_CHANNEL, f);
      if (elapsed >= 800) {
        _modem_phase = MODEM_STATIC_GAP;
        _modem_phase_start = millis();
        ledcWriteTone(BUZZER_CHANNEL, 0);
      }
      break;
    }

    case MODEM_STATIC_GAP:
      if (elapsed >= 100) {
        _modem_phase = MODEM_HANDSHAKE;
        _modem_phase_start = millis();
      }
      break;

    case MODEM_HANDSHAKE: {
      // Final handshake: sweep from 2000 down to 400 Hz
      uint32_t f = 2000 - ((elapsed * 2) % 1600);
      ledcWriteTone(BUZZER_CHANNEL, f);
      if (elapsed >= 1000) {
        _modem_phase = MODEM_DONE;
        modem_connect_stop();
      }
      break;
    }

    case MODEM_DONE:
      break;
  }
}
