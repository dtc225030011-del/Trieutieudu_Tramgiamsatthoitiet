#include "relay.h"
#include "cau_hinh.h"

static bool trangThaiQuat = false;
static bool trangThaiDen = false;

void khoiTaoRelay()
{
    pinMode(CHAN_RELAY_QUAT, OUTPUT);
    pinMode(CHAN_RELAY_DEN, OUTPUT);

    // Dam bao hai relay tat khi ESP32 khoi dong
    tatQuat();
    tatDen();
}

void batQuat()
{
    digitalWrite(CHAN_RELAY_QUAT, RELAY_BAT);
    trangThaiQuat = true;

    Serial.println("QUAT: BAT");
}

void tatQuat()
{
    digitalWrite(CHAN_RELAY_QUAT, RELAY_TAT);
    trangThaiQuat = false;

    Serial.println("QUAT: TAT");
}

void batDen()
{
    digitalWrite(CHAN_RELAY_DEN, RELAY_BAT);
    trangThaiDen = true;

    Serial.println("DEN: BAT");
}

void tatDen()
{
    digitalWrite(CHAN_RELAY_DEN, RELAY_TAT);
    trangThaiDen = false;

    Serial.println("DEN: TAT");
}

bool layTrangThaiQuat()
{
    return trangThaiQuat;
}

bool layTrangThaiDen()
{
    return trangThaiDen;
}