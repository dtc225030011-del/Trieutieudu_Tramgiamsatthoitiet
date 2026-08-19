#ifndef XU_LY_LENH_H
#define XU_LY_LENH_H

#include <Arduino.h>

// Che do dieu khien
enum CheDoDieuKhien
{
    TU_DONG,
    THU_CONG
};

// Xu ly ban tin MQTT nhan duoc
void xuLyBanTinMQTT(
    const char *topic,
    const byte *payload,
    unsigned int length
);

// Dieu khien tu dong dua tren cam bien
void xuLyDieuKhienTuDong(
    float nhietDo,
    float anhSang
);

// Lay nguong hien tai
float layNguongNhietDo();
float layNguongAnhSang();

// Lay che do hien tai
CheDoDieuKhien layCheDoDieuKhien();
const char *layTenCheDo();

#endif
