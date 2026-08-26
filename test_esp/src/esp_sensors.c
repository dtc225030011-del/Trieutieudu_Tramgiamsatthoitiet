#include "esp_sensors.h"

#include <stddef.h>

typedef struct {
    bool initialized;
    bool fault;
    bool timeout;
    float temperature_c;
    float pressure_pa;
} bmp280_mock_t;

typedef struct {
    bool initialized;
    bool fault;
    bool timeout;
    float light_lux;
} tsl2561_mock_t;

static bmp280_mock_t bmp;
static tsl2561_mock_t tsl;

void mock_bmp280_reset(void)
{
    bmp.initialized = false;
    bmp.fault = false;
    bmp.timeout = false;
    bmp.temperature_c = 25.0F;
    bmp.pressure_pa = 101325.0F;
}

void mock_bmp280_set_measurement(float temperature_c, float pressure_pa)
{
    bmp.temperature_c = temperature_c;
    bmp.pressure_pa = pressure_pa;
}

void mock_bmp280_set_fault(bool fault)
{
    bmp.fault = fault;
}

void mock_bmp280_set_timeout(bool timeout)
{
    bmp.timeout = timeout;
}

esp_sensor_status_t bmp280_init(void)
{
    if (bmp.fault) {
        return SENSOR_ERR_HARDWARE_FAULT;
    }
    bmp.initialized = true;
    return SENSOR_OK;
}

esp_sensor_status_t bmp280_read(float *temperature_c, float *pressure_hpa)
{
    float converted_pressure;

    if (temperature_c == NULL || pressure_hpa == NULL) {
        return SENSOR_ERR_INVALID_PARAM;
    }
    if (!bmp.initialized || bmp.fault) {
        return SENSOR_ERR_HARDWARE_FAULT;
    }
    if (bmp.timeout) {
        return SENSOR_ERR_TIMEOUT;
    }

    converted_pressure = bmp.pressure_pa / 100.0F;
    *temperature_c = bmp.temperature_c;
    *pressure_hpa = converted_pressure;

    if (bmp.temperature_c < BMP280_MIN_TEMPERATURE_C ||
        bmp.temperature_c > BMP280_MAX_TEMPERATURE_C ||
        converted_pressure < BMP280_MIN_PRESSURE_HPA ||
        converted_pressure > BMP280_MAX_PRESSURE_HPA) {
        return SENSOR_ERR_OUT_OF_RANGE;
    }
    return SENSOR_OK;
}

void mock_tsl2561_reset(void)
{
    tsl.initialized = false;
    tsl.fault = false;
    tsl.timeout = false;
    tsl.light_lux = 120.0F;
}

void mock_tsl2561_set_light(float light_lux)
{
    tsl.light_lux = light_lux;
}

void mock_tsl2561_set_fault(bool fault)
{
    tsl.fault = fault;
}

void mock_tsl2561_set_timeout(bool timeout)
{
    tsl.timeout = timeout;
}

esp_sensor_status_t tsl2561_init(void)
{
    if (tsl.fault) {
        return SENSOR_ERR_HARDWARE_FAULT;
    }
    tsl.initialized = true;
    return SENSOR_OK;
}

esp_sensor_status_t tsl2561_read_lux(float *light_lux)
{
    if (light_lux == NULL) {
        return SENSOR_ERR_INVALID_PARAM;
    }
    if (!tsl.initialized || tsl.fault) {
        return SENSOR_ERR_HARDWARE_FAULT;
    }
    if (tsl.timeout) {
        return SENSOR_ERR_TIMEOUT;
    }
    if (tsl.light_lux < 0.0F) {
        *light_lux = 0.0F;
        return SENSOR_OK;
    }

    *light_lux = tsl.light_lux;
    if (tsl.light_lux > TSL2561_MAX_LIGHT_LUX) {
        return SENSOR_ERR_OUT_OF_RANGE;
    }
    return SENSOR_OK;
}
