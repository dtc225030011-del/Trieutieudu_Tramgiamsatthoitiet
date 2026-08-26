#include "unity.h"
#include "esp_connectivity.h"
#include "esp_control.h"

#include <math.h>
#include <stdio.h>
#include <string.h>

static void connect_wifi(void)
{
    TEST_ASSERT_EQUAL_INT(CONN_STATUS_CONNECTED,
                          esp_wifi_connect("WeatherLab_AP", "weather123"));
}

static void connect_mqtt(void)
{
    connect_wifi();
    TEST_ASSERT_EQUAL_INT(
        CONN_STATUS_CONNECTED,
        esp_mqtt_connect("tram_thoi_tiet_esp32", "esp32", "123"));
}

void test_wifi_init_success(void)
{
    printf("   [NET-01] Khoi tao Wi-Fi o trang thai chua ket noi\n");
    mock_connectivity_reset();
    TEST_ASSERT_EQUAL_INT(CONN_STATUS_DISCONNECTED, esp_wifi_init());
    TEST_ASSERT_FALSE(esp_wifi_is_connected());
}

void test_wifi_connect_success(void)
{
    printf("   [NET-02] Ket noi Wi-Fi voi SSID/mat khau dung\n");
    connect_wifi();
    TEST_ASSERT_TRUE(esp_wifi_is_connected());
}

void test_wifi_authentication_failure(void)
{
    printf("   [NET-03] Bao loi xac thuc khi sai mat khau Wi-Fi\n");
    TEST_ASSERT_EQUAL_INT(CONN_STATUS_AUTH_FAIL,
                          esp_wifi_connect("WeatherLab_AP", "wrong-password"));
    TEST_ASSERT_FALSE(esp_wifi_is_connected());
}

void test_wifi_access_point_timeout(void)
{
    printf("   [NET-04] Bao timeout khi access point khong san sang\n");
    mock_wifi_set_access_point(false, "WeatherLab_AP", "weather123");
    TEST_ASSERT_EQUAL_INT(CONN_STATUS_TIMEOUT,
                          esp_wifi_connect("WeatherLab_AP", "weather123"));
}

void test_mqtt_requires_wifi_connection(void)
{
    printf("   [NET-05] MQTT khong ket noi khi Wi-Fi dang mat\n");
    TEST_ASSERT_EQUAL_INT(
        CONN_STATUS_TIMEOUT,
        esp_mqtt_connect("tram_thoi_tiet_esp32", "esp32", "123"));
}

void test_mqtt_authentication_failure(void)
{
    printf("   [NET-06] Bao loi khi sai tai khoan MQTT\n");
    connect_wifi();
    TEST_ASSERT_EQUAL_INT(
        CONN_STATUS_AUTH_FAIL,
        esp_mqtt_connect("tram_thoi_tiet_esp32", "esp32", "wrong"));
}

void test_mqtt_connect_subscribe_and_last_will(void)
{
    printf("   [NET-07] Ket noi, LWT va 3 topic subscribe dung cau hinh firmware\n");
    connect_mqtt();
    TEST_ASSERT_TRUE(esp_mqtt_is_connected());
    TEST_ASSERT_EQUAL_STRING(TOPIC_STATUS, esp_mqtt_last_will_topic());
    TEST_ASSERT_EQUAL_STRING("OFFLINE", esp_mqtt_last_will_payload());
    TEST_ASSERT_EQUAL_STRING("ONLINE", esp_mqtt_online_payload());
    TEST_ASSERT_TRUE(esp_mqtt_is_subscribed(TOPIC_CONTROL));
    TEST_ASSERT_TRUE(esp_mqtt_is_subscribed(TOPIC_THRESHOLD));
    TEST_ASSERT_TRUE(esp_mqtt_is_subscribed(TOPIC_MODE));
}

void test_mqtt_publish_weather_telemetry(void)
{
    const char *payload;

    printf("   [NET-08] Gui du lieu thoi tiet JSON len topic data\n");
    connect_mqtt();
    TEST_ASSERT_EQUAL_INT(
        CONN_STATUS_CONNECTED,
        esp_mqtt_publish_telemetry(28.5F, 1013.25F, 120.0F, false, true,
                                   "TU_DONG"));
    payload = mock_mqtt_last_payload();
    TEST_ASSERT_EQUAL_STRING(TOPIC_DATA, mock_mqtt_last_topic());
    TEST_ASSERT_TRUE(strstr(payload, "\"nhiet_do\":28.50") != NULL);
    TEST_ASSERT_TRUE(strstr(payload, "\"ap_suat\":1013.25") != NULL);
    TEST_ASSERT_TRUE(strstr(payload, "\"anh_sang\":120.00") != NULL);
    TEST_ASSERT_TRUE(strstr(payload, "\"quat\":0") != NULL);
    TEST_ASSERT_TRUE(strstr(payload, "\"den\":1") != NULL);
    TEST_ASSERT_TRUE(strstr(payload, "\"nguong_nhiet_do\":32.50") != NULL);
    TEST_ASSERT_TRUE(strstr(payload, "\"nguong_anh_sang\":55.00") != NULL);
    TEST_ASSERT_TRUE(strstr(payload, "\"che_do\":\"TU_DONG\"") != NULL);
}

void test_mqtt_publish_nan_as_json_null(void)
{
    const char *payload;

    printf("   [NET-09] Du lieu cam bien loi (NaN) duoc gui thanh JSON null\n");
    connect_mqtt();
    TEST_ASSERT_EQUAL_INT(
        CONN_STATUS_CONNECTED,
        esp_mqtt_publish_telemetry(NAN, NAN, NAN, false, false, "TU_DONG"));
    payload = mock_mqtt_last_payload();
    TEST_ASSERT_TRUE(strstr(payload, "\"nhiet_do\":null") != NULL);
    TEST_ASSERT_TRUE(strstr(payload, "\"ap_suat\":null") != NULL);
    TEST_ASSERT_TRUE(strstr(payload, "\"anh_sang\":null") != NULL);
}

void test_mqtt_publish_while_disconnected(void)
{
    printf("   [NET-10] Chan gui telemetry khi MQTT chua ket noi\n");
    TEST_ASSERT_EQUAL_INT(
        CONN_STATUS_DISCONNECTED,
        esp_mqtt_publish_telemetry(28.5F, 1013.25F, 120.0F, false, false,
                                   "TU_DONG"));
}

void test_mqtt_auto_reconnect(void)
{
    printf("   [NET-11] Tu dong ket noi lai MQTT sau khi mat ket noi\n");
    connect_mqtt();
    esp_mqtt_disconnect();
    TEST_ASSERT_EQUAL_INT(CONN_STATUS_CONNECTED, esp_mqtt_auto_reconnect(3));
    TEST_ASSERT_TRUE(esp_mqtt_is_connected());
}
