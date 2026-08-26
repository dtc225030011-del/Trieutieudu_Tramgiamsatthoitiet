#ifndef ESP_CONTROL_H
#define ESP_CONTROL_H

#include <stdbool.h>

#define DEFAULT_TEMPERATURE_THRESHOLD_C 32.5F
#define DEFAULT_LIGHT_THRESHOLD_LUX 55.0F

#define TOPIC_CONTROL "tramthoitiet/control"
#define TOPIC_THRESHOLD "tramthoitiet/threshold"
#define TOPIC_MODE "tramthoitiet/mode"

typedef enum {
    CONTROL_OK = 0,
    CONTROL_IGNORED = 1,
    CONTROL_INVALID_MESSAGE = -1,
    CONTROL_UNKNOWN_TOPIC = -2
} control_status_t;

typedef enum {
    MODE_AUTOMATIC = 0,
    MODE_MANUAL = 1
} control_mode_t;

void relay_init(void);
void fan_on(void);
void fan_off(void);
void light_on(void);
void light_off(void);
bool fan_is_on(void);
bool light_is_on(void);

void control_reset(void);
void control_automatic(float temperature_c, float light_lux);
control_status_t control_process_message(const char *topic, const char *payload);
float control_temperature_threshold(void);
float control_light_threshold(void);
control_mode_t control_mode(void);
const char *control_mode_name(void);

#endif
