#include "esp_connectivity.h"

#include "esp_control.h"

#include <math.h>
#include <stdio.h>
#include <string.h>

typedef struct {
    bool wifi_initialized;
    bool wifi_connected;
    bool ap_available;
    char ssid[33];
    char wifi_password[65];
    bool mqtt_initialized;
    bool mqtt_connected;
    bool broker_available;
    char mqtt_username[33];
    char mqtt_password[65];
    char broker[65];
    int port;
    char client_id[65];
    char last_topic[65];
    char last_payload[512];
} connectivity_mock_t;

static connectivity_mock_t connection;

static void copy_text(char *destination, size_t capacity, const char *source)
{
    if (capacity == 0U) {
        return;
    }
    if (source == NULL) {
        destination[0] = '\0';
        return;
    }
    snprintf(destination, capacity, "%s", source);
}

void mock_connectivity_reset(void)
{
    memset(&connection, 0, sizeof(connection));
    connection.ap_available = true;
    connection.broker_available = true;
    copy_text(connection.ssid, sizeof(connection.ssid), "WeatherLab_AP");
    copy_text(connection.wifi_password, sizeof(connection.wifi_password), "weather123");
    copy_text(connection.mqtt_username, sizeof(connection.mqtt_username), "esp32");
    copy_text(connection.mqtt_password, sizeof(connection.mqtt_password), "123");
}

void mock_wifi_set_access_point(bool available, const char *ssid,
                                const char *password)
{
    connection.ap_available = available;
    copy_text(connection.ssid, sizeof(connection.ssid), ssid);
    copy_text(connection.wifi_password, sizeof(connection.wifi_password), password);
}

void mock_mqtt_set_broker(bool available, const char *username,
                          const char *password)
{
    connection.broker_available = available;
    copy_text(connection.mqtt_username, sizeof(connection.mqtt_username), username);
    copy_text(connection.mqtt_password, sizeof(connection.mqtt_password), password);
}

esp_conn_status_t esp_wifi_init(void)
{
    connection.wifi_initialized = true;
    connection.wifi_connected = false;
    return CONN_STATUS_DISCONNECTED;
}

esp_conn_status_t esp_wifi_connect(const char *ssid, const char *password)
{
    if (!connection.wifi_initialized) {
        return CONN_STATUS_NOT_INITIALIZED;
    }
    if (ssid == NULL || password == NULL || *ssid == '\0') {
        return CONN_STATUS_INVALID_PARAM;
    }
    if (!connection.ap_available || strcmp(ssid, connection.ssid) != 0) {
        return CONN_STATUS_TIMEOUT;
    }
    if (strcmp(password, connection.wifi_password) != 0) {
        return CONN_STATUS_AUTH_FAIL;
    }
    connection.wifi_connected = true;
    return CONN_STATUS_CONNECTED;
}

esp_conn_status_t esp_wifi_disconnect(void)
{
    if (!connection.wifi_initialized) {
        return CONN_STATUS_NOT_INITIALIZED;
    }
    connection.wifi_connected = false;
    connection.mqtt_connected = false;
    return CONN_STATUS_DISCONNECTED;
}

bool esp_wifi_is_connected(void) { return connection.wifi_connected; }

esp_conn_status_t esp_mqtt_init(const char *broker, int port)
{
    if (broker == NULL || *broker == '\0' || port <= 0 || port > 65535) {
        return CONN_STATUS_INVALID_PARAM;
    }
    connection.mqtt_initialized = true;
    connection.mqtt_connected = false;
    copy_text(connection.broker, sizeof(connection.broker), broker);
    connection.port = port;
    return CONN_STATUS_DISCONNECTED;
}

esp_conn_status_t esp_mqtt_connect(const char *client_id, const char *username,
                                   const char *password)
{
    if (!connection.mqtt_initialized) {
        return CONN_STATUS_NOT_INITIALIZED;
    }
    if (!connection.wifi_connected || !connection.broker_available) {
        return CONN_STATUS_TIMEOUT;
    }
    if (client_id == NULL || username == NULL || password == NULL ||
        *client_id == '\0') {
        return CONN_STATUS_INVALID_PARAM;
    }
    if (strcmp(username, connection.mqtt_username) != 0 ||
        strcmp(password, connection.mqtt_password) != 0) {
        return CONN_STATUS_AUTH_FAIL;
    }
    copy_text(connection.client_id, sizeof(connection.client_id), client_id);
    connection.mqtt_connected = true;
    return CONN_STATUS_CONNECTED;
}

esp_conn_status_t esp_mqtt_disconnect(void)
{
    if (!connection.mqtt_initialized) {
        return CONN_STATUS_NOT_INITIALIZED;
    }
    connection.mqtt_connected = false;
    return CONN_STATUS_DISCONNECTED;
}

esp_conn_status_t esp_mqtt_auto_reconnect(int max_retries)
{
    int attempt;

    if (max_retries <= 0) {
        return CONN_STATUS_INVALID_PARAM;
    }
    if (connection.mqtt_connected) {
        return CONN_STATUS_CONNECTED;
    }
    for (attempt = 0; attempt < max_retries; attempt++) {
        esp_conn_status_t status = esp_mqtt_connect(
            connection.client_id[0] ? connection.client_id : "tram_thoi_tiet_esp32",
            connection.mqtt_username, connection.mqtt_password);
        if (status == CONN_STATUS_CONNECTED) {
            return status;
        }
    }
    return CONN_STATUS_TIMEOUT;
}

bool esp_mqtt_is_connected(void) { return connection.mqtt_connected; }

bool esp_mqtt_is_subscribed(const char *topic)
{
    if (!connection.mqtt_connected || topic == NULL) {
        return false;
    }
    return strcmp(topic, TOPIC_CONTROL) == 0 || strcmp(topic, TOPIC_THRESHOLD) == 0 ||
           strcmp(topic, TOPIC_MODE) == 0;
}

const char *esp_mqtt_last_will_topic(void) { return TOPIC_STATUS; }
const char *esp_mqtt_last_will_payload(void) { return "OFFLINE"; }
const char *esp_mqtt_online_payload(void) { return "ONLINE"; }

static void append_measurement(char *buffer, size_t capacity, const char *key,
                               float value, bool comma)
{
    const size_t used = strlen(buffer);
    if (isnan(value)) {
        snprintf(buffer + used, capacity - used, "\"%s\":null%s", key,
                 comma ? "," : "");
    } else {
        snprintf(buffer + used, capacity - used, "\"%s\":%.2f%s", key, value,
                 comma ? "," : "");
    }
}

esp_conn_status_t esp_mqtt_publish_telemetry(float temperature_c,
                                             float pressure_hpa,
                                             float light_lux,
                                             bool fan_on_state,
                                             bool light_on_state,
                                             const char *mode_name)
{
    size_t used;

    if (!connection.mqtt_connected) {
        return CONN_STATUS_DISCONNECTED;
    }
    if (mode_name == NULL) {
        return CONN_STATUS_INVALID_PARAM;
    }
    copy_text(connection.last_topic, sizeof(connection.last_topic), TOPIC_DATA);
    copy_text(connection.last_payload, sizeof(connection.last_payload), "{");
    append_measurement(connection.last_payload, sizeof(connection.last_payload),
                       "nhiet_do", temperature_c, true);
    append_measurement(connection.last_payload, sizeof(connection.last_payload),
                       "ap_suat", pressure_hpa, true);
    append_measurement(connection.last_payload, sizeof(connection.last_payload),
                       "anh_sang", light_lux, true);
    used = strlen(connection.last_payload);
    snprintf(connection.last_payload + used, sizeof(connection.last_payload) - used,
             "\"quat\":%d,\"den\":%d,\"nguong_nhiet_do\":%.2f,"
             "\"nguong_anh_sang\":%.2f,\"che_do\":\"%s\"}",
             fan_on_state ? 1 : 0, light_on_state ? 1 : 0,
             control_temperature_threshold(), control_light_threshold(), mode_name);
    return CONN_STATUS_CONNECTED;
}

const char *mock_mqtt_last_topic(void) { return connection.last_topic; }
const char *mock_mqtt_last_payload(void) { return connection.last_payload; }
