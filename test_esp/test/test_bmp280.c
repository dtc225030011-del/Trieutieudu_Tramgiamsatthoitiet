#include "unity.h"
#include "esp_sensors.h"

#include <stdio.h>

void test_bmp280_init_success(void)
{
    printf("   [BMP280-01] Khoi tao BMP280 tai dia chi I2C mo phong 0x76\n");
    mock_bmp280_reset();
    TEST_ASSERT_EQUAL_INT(SENSOR_OK, bmp280_init());
}

void test_bmp280_init_hardware_fault(void)
{
    printf("   [BMP280-02] Phat hien cam bien mat ket noi tren bus I2C\n");
    mock_bmp280_reset();
    mock_bmp280_set_fault(true);
    TEST_ASSERT_EQUAL_INT(SENSOR_ERR_HARDWARE_FAULT, bmp280_init());
}

void test_bmp280_read_normal_measurement(void)
{
    float temperature = 0.0F;
    float pressure = 0.0F;

    printf("   [BMP280-03] Doc nhiet do 28.50 C va ap suat 1013.25 hPa\n");
    mock_bmp280_set_measurement(28.5F, 101325.0F);
    TEST_ASSERT_EQUAL_INT(SENSOR_OK, bmp280_read(&temperature, &pressure));
    TEST_ASSERT_FLOAT_WITHIN(0.01F, 28.5F, temperature);
    TEST_ASSERT_FLOAT_WITHIN(0.01F, 1013.25F, pressure);
}

void test_bmp280_pressure_conversion_pa_to_hpa(void)
{
    float temperature = 0.0F;
    float pressure = 0.0F;

    printf("   [BMP280-04] Chuyen doi 100000 Pa thanh 1000 hPa\n");
    mock_bmp280_set_measurement(25.0F, 100000.0F);
    TEST_ASSERT_EQUAL_INT(SENSOR_OK, bmp280_read(&temperature, &pressure));
    TEST_ASSERT_FLOAT_WITHIN(0.01F, 1000.0F, pressure);
}

void test_bmp280_temperature_out_of_range(void)
{
    float temperature = 0.0F;
    float pressure = 0.0F;

    printf("   [BMP280-05] Chan gia tri nhiet do vuot dai do\n");
    mock_bmp280_set_measurement(90.0F, 101325.0F);
    TEST_ASSERT_EQUAL_INT(SENSOR_ERR_OUT_OF_RANGE,
                          bmp280_read(&temperature, &pressure));
}

void test_bmp280_pressure_out_of_range(void)
{
    float temperature = 0.0F;
    float pressure = 0.0F;

    printf("   [BMP280-06] Chan gia tri ap suat vuot dai do\n");
    mock_bmp280_set_measurement(25.0F, 120000.0F);
    TEST_ASSERT_EQUAL_INT(SENSOR_ERR_OUT_OF_RANGE,
                          bmp280_read(&temperature, &pressure));
}

void test_bmp280_read_timeout(void)
{
    float temperature = 0.0F;
    float pressure = 0.0F;

    printf("   [BMP280-07] Xu ly timeout khi bus I2C khong phan hoi\n");
    mock_bmp280_set_timeout(true);
    TEST_ASSERT_EQUAL_INT(SENSOR_ERR_TIMEOUT,
                          bmp280_read(&temperature, &pressure));
}

void test_bmp280_null_pointer_safety(void)
{
    float pressure = 0.0F;

    printf("   [BMP280-08] Bao ve bo nho khi output la NULL\n");
    TEST_ASSERT_EQUAL_INT(SENSOR_ERR_INVALID_PARAM,
                          bmp280_read(NULL, &pressure));
}
