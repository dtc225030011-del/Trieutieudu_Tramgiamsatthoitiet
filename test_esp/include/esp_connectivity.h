#ifndef ESP_CONNECTIVITY_H
#define ESP_CONNECTIVITY_H

#include <stdbool.h>

#define TOPIC_DATA "tramthoitiet/data"
#define TOPIC_STATUS "tramthoitiet/status"

typedef enum {
    CONN_STATUS_DISCONNECTED = 0,
    CONN_STATUS_CONNECTED = 1,
    CONN_STATUS_AUTH_FAIL = -1,
    CONN_STATUS_TIMEOUT = -2,
    CONN_STATUS_NOT_INITIALIZED = -3,
    CONN_STATUS_INVALID_PARAM = -4
} esp_conn_status_t;

void mock_connectivity_reset(void);
void mock_wifi_set_access_point(bool available, const char *ssid,
                                const char *password);
void mock_mqtt_set_broker(bool available, const char *username,
                          const char *password);

esp_conn_status_t esp_wifi_init(void);
esp_conn_status_t esp_wifi_connect(const char *ssid, const char *password);
esp_conn_status_t esp_wifi_disconnect(void);
bool esp_wifi_is_connected(void);

esp_conn_status_t esp_mqtt_init(const char *broker, int port);
esp_conn_status_t esp_mqtt_connect(const char *client_id, const char *username,
                                   const char *password);
esp_conn_status_t esp_mqtt_disconnect(void);
esp_conn_status_t esp_mqtt_auto_reconnect(int max_retries);
bool esp_mqtt_is_connected(void);
bool esp_mqtt_is_subscribed(const char *topic);
const char *esp_mqtt_last_will_topic(void);
const char *esp_mqtt_last_will_payload(void);
const char *esp_mqtt_online_payload(void);

esp_conn_status_t esp_mqtt_publish_telemetry(float temperature_c,
                                             float pressure_hpa,
                                             float light_lux,
                                             bool fan_on_state,
                                             bool light_on_state,
                                             const char *mode_name);
const char *mock_mqtt_last_topic(void);
const char *mock_mqtt_last_payload(void);

#endif
