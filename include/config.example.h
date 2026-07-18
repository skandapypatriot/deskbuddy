#ifndef CONFIG_H
#define CONFIG_H

// Wi-Fi Connection Settings
struct WifiNetwork {
    const char* ssid;
    const char* password;
};

// Add your local Wi-Fi networks here.
// The device will try to connect to them in order.
const WifiNetwork wifi_networks[] = {
    {"YOUR_SSID", "YOUR_PASSWORD"},
};

// NTP Configuration
const char* ntp_server = "pool.ntp.org";
const long gmt_offset_sec = 19800; // GMT +5:30 (Indian Standard Time)
const int daylight_offset_sec = 0;  // No DST in India

// WeatherAPI.com Configuration
// Register at https://www.weatherapi.com to get your free API key.
const char* weather_api_key = "YOUR_API_KEY";
const char* weather_location = "YOUR_CITY";
#endif // CONFIG_H
