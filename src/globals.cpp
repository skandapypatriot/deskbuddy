#include "globals.h"

U8G2_SSD1306_128X64_NONAME_1_HW_I2C disp(U8G2_R0, U8X8_PIN_NONE);
RTC_DS3231 rtc;
bool     rtc_began    = false;
DateTime now;
bool     wifi_connected = false;
int      current_screen = 0;
bool     pomo_screen_active = false;
unsigned long last_screen_switch = 0;

float weather_temp     = 0.0f;
int   weather_humidity = 0;
char  weather_desc[32] = "loading...";
char  weather_loc[32]  = "loading...";
unsigned long last_weather_fetch = 0;
int   weather_code     = 1000;

int   forecastCodes[3]    = {1000, 1000, 1000};
float forecastHighs[3]    = {0, 0, 0};
float forecastLows[3]     = {0, 0, 0};
int   forecastRain[3]     = {0, 0, 0};
char  forecastDays[3][4]  = {"D1", "D2", "D3"};

const uint8_t POMO_PRESETS[]   = {5, 10, 15, 20, 25, 30, 35, 40, 45, 50};
const uint8_t POMO_NUM_PRESETS = sizeof(POMO_PRESETS);

uint8_t       pomo_preset_idx  = 4;
bool          pomo_running     = false;
unsigned long pomo_start_ms    = 0;
unsigned long pomo_duration_ms = 0;
bool          pomo_done        = false;

bool          btn_was_pressed  = false;
unsigned long btn_press_start  = 0;
bool          btn_long_fired   = false;
