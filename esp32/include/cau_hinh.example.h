#ifndef CAU_HINH_EXAMPLE_H
#define CAU_HINH_EXAMPLE_H

#include <Arduino.h>

// ==================================================
// SERIAL
// ==================================================
#define TOC_DO_SERIAL 115200

// ==================================================
// I2C PINS (ESP32)
// ==================================================
#define CHAN_SDA 21
#define CHAN_SCL 22

// ==================================================
// RELAY GPIO PINS
// ==================================================
#define CHAN_RELAY_QUAT 26
#define CHAN_RELAY_DEN  27

// Active HIGH (HIGH = BẬT, LOW = TẮT)
#define RELAY_BAT HIGH
#define RELAY_TAT LOW

// ==================================================
// WI-FI CONFIGURATION
// ==================================================
#define TEN_WIFI       "YOUR_WIFI_SSID"
#define MAT_KHAU_WIFI  "YOUR_WIFI_PASSWORD"

// ==================================================
// MQTT BROKER CONFIGURATION
// ==================================================
#define MQTT_BROKER    "192.168.1.100" // IP của máy tính / Raspberry Pi
#define MQTT_PORT      1883

#define MQTT_USERNAME  "esp32"
#define MQTT_PASSWORD  "123"

#define MQTT_CLIENT_ID "tram_thoi_tiet_esp32"

// MQTT Topics
#define TOPIC_DU_LIEU    "tramthoitiet/data"
#define TOPIC_TRANG_THAI "tramthoitiet/status"
#define TOPIC_DIEU_KHIEN "tramthoitiet/control"
#define TOPIC_NGUONG     "tramthoitiet/threshold"
#define TOPIC_CHE_DO     "tramthoitiet/mode"

// ==================================================
// DEFAULT THRESHOLDS
// ==================================================
#define NGUONG_NHIET_DO_MAC_DINH 32.5F
#define NGUONG_ANH_SANG_MAC_DINH 55.0F

// ==================================================
// TIMINGS (ms)
// ==================================================
#define CHU_KY_GUI_DU_LIEU      2000UL
#define CHU_KY_KET_NOI_LAI_MQTT 5000UL
#define CHU_KY_KET_NOI_LAI_WIFI 10000UL

#endif // CAU_HINH_EXAMPLE_H
