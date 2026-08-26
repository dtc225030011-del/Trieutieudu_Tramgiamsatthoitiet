#include "unity.h"
#include "esp_control.h"

#include <math.h>
#include <stdio.h>

void test_relay_init_safe_off(void)
{
    printf("   [CTRL-01] Hai relay tat an toan sau khoi dong\n");
    fan_on();
    light_on();
    relay_init();
    TEST_ASSERT_FALSE(fan_is_on());
    TEST_ASSERT_FALSE(light_is_on());
}

void test_fan_manual_state_functions(void)
{
    printf("   [CTRL-02] Ham bat/tat quat cap nhat dung trang thai\n");
    fan_on();
    TEST_ASSERT_TRUE(fan_is_on());
    fan_off();
    TEST_ASSERT_FALSE(fan_is_on());
}

void test_light_manual_state_functions(void)
{
    printf("   [CTRL-03] Ham bat/tat den cap nhat dung trang thai\n");
    light_on();
    TEST_ASSERT_TRUE(light_is_on());
    light_off();
    TEST_ASSERT_FALSE(light_is_on());
}

void test_automatic_hot_temperature_turns_fan_on(void)
{
    printf("   [CTRL-04] Nhiet do cao hon nguong lam quat bat\n");
    control_automatic(35.0F, 100.0F);
    TEST_ASSERT_TRUE(fan_is_on());
}

void test_automatic_low_temperature_turns_fan_off(void)
{
    printf("   [CTRL-05] Nhiet do bang/thap hon nguong lam quat tat\n");
    fan_on();
    control_automatic(DEFAULT_TEMPERATURE_THRESHOLD_C, 100.0F);
    TEST_ASSERT_FALSE(fan_is_on());
}

void test_automatic_dark_environment_turns_light_on(void)
{
    printf("   [CTRL-06] Anh sang thap hon nguong lam den bat\n");
    control_automatic(25.0F, 20.0F);
    TEST_ASSERT_TRUE(light_is_on());
}

void test_automatic_bright_environment_turns_light_off(void)
{
    printf("   [CTRL-07] Anh sang bang/cao hon nguong lam den tat\n");
    light_on();
    control_automatic(25.0F, DEFAULT_LIGHT_THRESHOLD_LUX);
    TEST_ASSERT_FALSE(light_is_on());
}

void test_nan_sensor_value_preserves_relay_state(void)
{
    printf("   [CTRL-08] Du lieu NaN khong lam thay doi relay\n");
    fan_on();
    light_on();
    control_automatic(NAN, NAN);
    TEST_ASSERT_TRUE(fan_is_on());
    TEST_ASSERT_TRUE(light_is_on());
}

void test_manual_mode_ignores_automatic_control(void)
{
    printf("   [CTRL-09] Che do THU_CONG bo qua dieu khien tu dong\n");
    TEST_ASSERT_EQUAL_INT(CONTROL_OK,
                          control_process_message(TOPIC_MODE, "THU_CONG"));
    control_automatic(40.0F, 1.0F);
    TEST_ASSERT_FALSE(fan_is_on());
    TEST_ASSERT_FALSE(light_is_on());
}

void test_manual_fan_command(void)
{
    printf("   [CTRL-10] Lenh MQTT bat quat chi chay o che do THU_CONG\n");
    control_process_message(TOPIC_MODE, "THU_CONG");
    TEST_ASSERT_EQUAL_INT(
        CONTROL_OK,
        control_process_message(TOPIC_CONTROL,
                                "{\"device\":\"fan\",\"command\":\"ON\"}"));
    TEST_ASSERT_TRUE(fan_is_on());
}

void test_manual_light_command(void)
{
    printf("   [CTRL-11] Lenh MQTT bat den xu ly dung device/command\n");
    control_process_message(TOPIC_MODE, "THU_CONG");
    TEST_ASSERT_EQUAL_INT(
        CONTROL_OK,
        control_process_message(TOPIC_CONTROL,
                                "{\"device\":\"light\",\"command\":\"ON\"}"));
    TEST_ASSERT_TRUE(light_is_on());
}

void test_control_command_ignored_in_automatic_mode(void)
{
    printf("   [CTRL-12] Lenh dieu khien tay bi bo qua o che do TU_DONG\n");
    TEST_ASSERT_EQUAL_INT(
        CONTROL_IGNORED,
        control_process_message(TOPIC_CONTROL,
                                "{\"device\":\"fan\",\"command\":\"ON\"}"));
    TEST_ASSERT_FALSE(fan_is_on());
}

void test_threshold_message_updates_valid_values(void)
{
    printf("   [CTRL-13] Cap nhat hai nguong tu MQTT JSON hop le\n");
    TEST_ASSERT_EQUAL_INT(
        CONTROL_OK,
        control_process_message(
            TOPIC_THRESHOLD,
            "{\"nguong_nhiet_do\":30.0,\"nguong_anh_sang\":75.0}"));
    TEST_ASSERT_FLOAT_WITHIN(0.01F, 30.0F, control_temperature_threshold());
    TEST_ASSERT_FLOAT_WITHIN(0.01F, 75.0F, control_light_threshold());
}

void test_invalid_threshold_keeps_defaults(void)
{
    printf("   [CTRL-14] Nguong ngoai mien hop le khong duoc ap dung\n");
    TEST_ASSERT_EQUAL_INT(
        CONTROL_INVALID_MESSAGE,
        control_process_message(
            TOPIC_THRESHOLD,
            "{\"nguong_nhiet_do\":150.0,\"nguong_anh_sang\":-1.0}"));
    TEST_ASSERT_FLOAT_WITHIN(0.01F, DEFAULT_TEMPERATURE_THRESHOLD_C,
                             control_temperature_threshold());
    TEST_ASSERT_FLOAT_WITHIN(0.01F, DEFAULT_LIGHT_THRESHOLD_LUX,
                             control_light_threshold());
}

void test_unknown_topic_is_rejected(void)
{
    printf("   [CTRL-15] Topic MQTT khong biet duoc tu choi an toan\n");
    TEST_ASSERT_EQUAL_INT(CONTROL_UNKNOWN_TOPIC,
                          control_process_message("tramthoitiet/unknown", "ON"));
}
