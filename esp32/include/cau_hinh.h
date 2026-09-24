#ifndef CAU_HINH_H
#define CAU_HINH_H

#include <Arduino.h>

// ==================================================
// SERIAL
// ==================================================

#define TOC_DO_SERIAL 115200

// ==================================================
// I2C
// ==================================================

#define CHAN_SDA 21
#define CHAN_SCL 22

// ==================================================
// RELAY
// ==================================================

#define CHAN_RELAY_QUAT 26
#define CHAN_RELAY_DEN 27

// Relay cua ban kich muc HIGH
#define RELAY_BAT HIGH
#define RELAY_TAT LOW

// ==================================================
// WIFI
// ==================================================

#define TEN_WIFI "ESPTEST"
#define MAT_KHAU_WIFI "88888888"

// ==================================================
// MQTT
// ==================================================

#define MQTT_BROKER "192.168.137.227"
#define MQTT_PORT 1883

#define MQTT_USERNAME "esp32"
#define MQTT_PASSWORD "123"

#define MQTT_CLIENT_ID "tram_thoi_tiet_esp32"

// ESP32 gui
#define TOPIC_DU_LIEU "tramthoitiet/data"
#define TOPIC_TRANG_THAI "tramthoitiet/status"

// ESP32 nhan
#define TOPIC_DIEU_KHIEN "tramthoitiet/control"
#define TOPIC_NGUONG "tramthoitiet/threshold"
#define TOPIC_CHE_DO "tramthoitiet/mode"

// ==================================================
// NGUONG MAC DINH
// ==================================================

#define NGUONG_NHIET_DO_MAC_DINH 32.5F
#define NGUONG_ANH_SANG_MAC_DINH 55.0F

// ==================================================
// THOI GIAN
// ==================================================

// Gui du lieu moi 2 giay
#define CHU_KY_GUI_DU_LIEU 2000UL

// Thu ket noi lai MQTT moi 5 giay
#define CHU_KY_KET_NOI_LAI_MQTT 5000UL

// Thu ket noi lai Wi-Fi moi 10 giay
#define CHU_KY_KET_NOI_LAI_WIFI 10000UL

#endif
