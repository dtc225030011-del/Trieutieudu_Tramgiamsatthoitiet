#ifndef ESP_SENSORS_H
#define ESP_SENSORS_H

#include <stdbool.h>

typedef enum {
    SENSOR_OK = 0,
    SENSOR_ERR_TIMEOUT = -1,
    SENSOR_ERR_OUT_OF_RANGE = -2,
    SENSOR_ERR_HARDWARE_FAULT = -3,
    SENSOR_ERR_INVALID_PARAM = -4
} esp_sensor_status_t;

#define BMP280_MIN_TEMPERATURE_C (-40.0F)
#define BMP280_MAX_TEMPERATURE_C 85.0F
#define BMP280_MIN_PRESSURE_HPA 300.0F
#define BMP280_MAX_PRESSURE_HPA 1100.0F
#define TSL2561_MAX_LIGHT_LUX 40000.0F

esp_sensor_status_t bmp280_init(void);
esp_sensor_status_t bmp280_read(float *temperature_c, float *pressure_hpa);
void mock_bmp280_reset(void);
void mock_bmp280_set_measurement(float temperature_c, float pressure_pa);
void mock_bmp280_set_fault(bool fault);
void mock_bmp280_set_timeout(bool timeout);

esp_sensor_status_t tsl2561_init(void);
esp_sensor_status_t tsl2561_read_lux(float *light_lux);
void mock_tsl2561_reset(void);
void mock_tsl2561_set_light(float light_lux);
void mock_tsl2561_set_fault(bool fault);
void mock_tsl2561_set_timeout(bool timeout);

#endif
