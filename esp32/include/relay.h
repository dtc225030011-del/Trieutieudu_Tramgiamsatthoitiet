#ifndef RELAY_H
#define RELAY_H

#include <Arduino.h>

// Khoi tao hai relay
void khoiTaoRelay();

// Dieu khien quat
void batQuat();
void tatQuat();

// Dieu khien den
void batDen();
void tatDen();

// Lay trang thai relay
bool layTrangThaiQuat();
bool layTrangThaiDen();

#endif