#include "tsl2561.h"

#include <Adafruit_Sensor.h>
#include <Adafruit_TSL2561_U.h>

static Adafruit_TSL2561_Unified tsl2561(
    TSL2561_ADDR_FLOAT,
    12345
);

bool khoiTaoTSL2561()
{
    if (!tsl2561.begin())
    {
        return false;
    }

    tsl2561.enableAutoRange(true);

    tsl2561.setIntegrationTime(
        TSL2561_INTEGRATIONTIME_101MS
    );

    return true;
}

float docAnhSangTSL2561()
{
    sensors_event_t suKien;

    tsl2561.getEvent(&suKien);

    if (suKien.light < 0)
    {
        return 0.0F;
    }

    return suKien.light;
}