#include "unity.h"
#include "esp_connectivity.h"
#include "esp_control.h"
#include "esp_sensors.h"

#include <stdio.h>

void setUp(void)
{
    mock_bmp280_reset();
    (void)bmp280_init();
    mock_tsl2561_reset();
    (void)tsl2561_init();
    control_reset();
    mock_connectivity_reset();
    (void)esp_wifi_init();
    (void)esp_mqtt_init("192.168.1.100", 1883);
}

void tearDown(void)
{
    (void)esp_mqtt_disconnect();
    (void)esp_wifi_disconnect();
}

extern void test_bmp280_init_success(void);
extern void test_bmp280_init_hardware_fault(void);
extern void test_bmp280_read_normal_measurement(void);
extern void test_bmp280_pressure_conversion_pa_to_hpa(void);
extern void test_bmp280_temperature_out_of_range(void);
extern void test_bmp280_pressure_out_of_range(void);
extern void test_bmp280_read_timeout(void);
extern void test_bmp280_null_pointer_safety(void);

extern void test_tsl2561_init_success(void);
extern void test_tsl2561_init_hardware_fault(void);
extern void test_tsl2561_read_normal_light(void);
extern void test_tsl2561_negative_light_clamped_to_zero(void);
extern void test_tsl2561_light_out_of_range(void);
extern void test_tsl2561_read_timeout(void);
extern void test_tsl2561_null_pointer_safety(void);

extern void test_relay_init_safe_off(void);
extern void test_fan_manual_state_functions(void);
extern void test_light_manual_state_functions(void);
extern void test_automatic_hot_temperature_turns_fan_on(void);
extern void test_automatic_low_temperature_turns_fan_off(void);
extern void test_automatic_dark_environment_turns_light_on(void);
extern void test_automatic_bright_environment_turns_light_off(void);
extern void test_nan_sensor_value_preserves_relay_state(void);
extern void test_manual_mode_ignores_automatic_control(void);
extern void test_manual_fan_command(void);
extern void test_manual_light_command(void);
extern void test_control_command_ignored_in_automatic_mode(void);
extern void test_threshold_message_updates_valid_values(void);
extern void test_invalid_threshold_keeps_defaults(void);
extern void test_unknown_topic_is_rejected(void);

extern void test_wifi_init_success(void);
extern void test_wifi_connect_success(void);
extern void test_wifi_authentication_failure(void);
extern void test_wifi_access_point_timeout(void);
extern void test_mqtt_requires_wifi_connection(void);
extern void test_mqtt_authentication_failure(void);
extern void test_mqtt_connect_subscribe_and_last_will(void);
extern void test_mqtt_publish_weather_telemetry(void);
extern void test_mqtt_publish_nan_as_json_null(void);
extern void test_mqtt_publish_while_disconnected(void);
extern void test_mqtt_auto_reconnect(void);

int main(void)
{
    UNITY_BEGIN();

    printf("\n======================================================================\n");
    printf(" [1] KIEM THU CAM BIEN BMP280 (8 TEST CASES)\n");
    printf("======================================================================\n");
    RUN_TEST(test_bmp280_init_success);
    RUN_TEST(test_bmp280_init_hardware_fault);
    RUN_TEST(test_bmp280_read_normal_measurement);
    RUN_TEST(test_bmp280_pressure_conversion_pa_to_hpa);
    RUN_TEST(test_bmp280_temperature_out_of_range);
    RUN_TEST(test_bmp280_pressure_out_of_range);
    RUN_TEST(test_bmp280_read_timeout);
    RUN_TEST(test_bmp280_null_pointer_safety);

    printf("\n======================================================================\n");
    printf(" [2] KIEM THU CAM BIEN ANH SANG TSL2561 (7 TEST CASES)\n");
    printf("======================================================================\n");
    RUN_TEST(test_tsl2561_init_success);
    RUN_TEST(test_tsl2561_init_hardware_fault);
    RUN_TEST(test_tsl2561_read_normal_light);
    RUN_TEST(test_tsl2561_negative_light_clamped_to_zero);
    RUN_TEST(test_tsl2561_light_out_of_range);
    RUN_TEST(test_tsl2561_read_timeout);
    RUN_TEST(test_tsl2561_null_pointer_safety);

    printf("\n======================================================================\n");
    printf(" [3] KIEM THU RELAY & DIEU KHIEN MQTT (15 TEST CASES)\n");
    printf("======================================================================\n");
    RUN_TEST(test_relay_init_safe_off);
    RUN_TEST(test_fan_manual_state_functions);
    RUN_TEST(test_light_manual_state_functions);
    RUN_TEST(test_automatic_hot_temperature_turns_fan_on);
    RUN_TEST(test_automatic_low_temperature_turns_fan_off);
    RUN_TEST(test_automatic_dark_environment_turns_light_on);
    RUN_TEST(test_automatic_bright_environment_turns_light_off);
    RUN_TEST(test_nan_sensor_value_preserves_relay_state);
    RUN_TEST(test_manual_mode_ignores_automatic_control);
    RUN_TEST(test_manual_fan_command);
    RUN_TEST(test_manual_light_command);
    RUN_TEST(test_control_command_ignored_in_automatic_mode);
    RUN_TEST(test_threshold_message_updates_valid_values);
    RUN_TEST(test_invalid_threshold_keeps_defaults);
    RUN_TEST(test_unknown_topic_is_rejected);

    printf("\n======================================================================\n");
    printf(" [4] KIEM THU KET NOI WI-FI & MQTT (11 TEST CASES)\n");
    printf("======================================================================\n");
    RUN_TEST(test_wifi_init_success);
    RUN_TEST(test_wifi_connect_success);
    RUN_TEST(test_wifi_authentication_failure);
    RUN_TEST(test_wifi_access_point_timeout);
    RUN_TEST(test_mqtt_requires_wifi_connection);
    RUN_TEST(test_mqtt_authentication_failure);
    RUN_TEST(test_mqtt_connect_subscribe_and_last_will);
    RUN_TEST(test_mqtt_publish_weather_telemetry);
    RUN_TEST(test_mqtt_publish_nan_as_json_null);
    RUN_TEST(test_mqtt_publish_while_disconnected);
    RUN_TEST(test_mqtt_auto_reconnect);

    return UNITY_END();
}
