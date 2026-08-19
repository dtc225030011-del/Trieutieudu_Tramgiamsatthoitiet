#ifndef WIFI_MQTT_H
#define WIFI_MQTT_H

#include <Arduino.h>

// Khoi tao Wi-Fi va MQTT
void khoiTaoWiFiMQTT();

// Xu ly ket noi Wi-Fi va MQTT trong loop
void xuLyWiFiMQTT();

// Gui du lieu cam bien va trang thai he thong
void guiDuLieuMQTT(
    float nhietDo,
    float apSuat,
    float anhSang
);

// Kiem tra ket noi
bool wifiDangKetNoi();
bool mqttDangKetNoi();

#endif