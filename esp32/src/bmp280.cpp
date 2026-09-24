#include "bmp280.h"

#include <Adafruit_BMP280.h>

static Adafruit_BMP280 bmp280;

bool khoiTaoBMP280()
{
    // Module BMP280 cua ban co dia chi 0x76
    if (!bmp280.begin(0x76))
    {
        return false;
    }

    bmp280.setSampling(
        Adafruit_BMP280::MODE_NORMAL,
        Adafruit_BMP280::SAMPLING_X2,
        Adafruit_BMP280::SAMPLING_X16,
        Adafruit_BMP280::FILTER_X16,
        Adafruit_BMP280::STANDBY_MS_500
    );

    return true;
}

float docNhietDoBMP280()
{
    return bmp280.readTemperature();
}

float docApSuatBMP280()
{
    return bmp280.readPressure() / 100.0F;
}