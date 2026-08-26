#include "unity.h"
#include "esp_sensors.h"

#include <stdio.h>

void test_tsl2561_init_success(void)
{
    printf("   [TSL2561-01] Khoi tao cam bien anh sang thanh cong\n");
    mock_tsl2561_reset();
    TEST_ASSERT_EQUAL_INT(SENSOR_OK, tsl2561_init());
}

void test_tsl2561_init_hardware_fault(void)
{
    printf("   [TSL2561-02] Phat hien loi phan cung cam bien anh sang\n");
    mock_tsl2561_reset();
    mock_tsl2561_set_fault(true);
    TEST_ASSERT_EQUAL_INT(SENSOR_ERR_HARDWARE_FAULT, tsl2561_init());
}

void test_tsl2561_read_normal_light(void)
{
    float light = 0.0F;

    printf("   [TSL2561-03] Doc cuong do sang 120 lux\n");
    mock_tsl2561_set_light(120.0F);
    TEST_ASSERT_EQUAL_INT(SENSOR_OK, tsl2561_read_lux(&light));
    TEST_ASSERT_FLOAT_WITHIN(0.01F, 120.0F, light);
}

void test_tsl2561_negative_light_clamped_to_zero(void)
{
    float light = -1.0F;

    printf("   [TSL2561-04] Gia tri am duoc dua ve 0 nhu firmware\n");
    mock_tsl2561_set_light(-3.0F);
    TEST_ASSERT_EQUAL_INT(SENSOR_OK, tsl2561_read_lux(&light));
    TEST_ASSERT_EQUAL_FLOAT(0.0F, light);
}

void test_tsl2561_light_out_of_range(void)
{
    float light = 0.0F;

    printf("   [TSL2561-05] Bao loi khi cuong do sang vuot dai cam bien\n");
    mock_tsl2561_set_light(50000.0F);
    TEST_ASSERT_EQUAL_INT(SENSOR_ERR_OUT_OF_RANGE, tsl2561_read_lux(&light));
}

void test_tsl2561_read_timeout(void)
{
    float light = 0.0F;

    printf("   [TSL2561-06] Xu ly timeout tren bus I2C\n");
    mock_tsl2561_set_timeout(true);
    TEST_ASSERT_EQUAL_INT(SENSOR_ERR_TIMEOUT, tsl2561_read_lux(&light));
}

void test_tsl2561_null_pointer_safety(void)
{
    printf("   [TSL2561-07] Bao ve bo nho khi output la NULL\n");
    TEST_ASSERT_EQUAL_INT(SENSOR_ERR_INVALID_PARAM, tsl2561_read_lux(NULL));
}
