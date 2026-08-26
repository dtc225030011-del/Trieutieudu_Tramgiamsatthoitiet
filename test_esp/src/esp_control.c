#include "esp_control.h"

#include <ctype.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

static bool fan_state;
static bool light_state;
static float temperature_threshold;
static float light_threshold;
static control_mode_t current_mode;

void relay_init(void)
{
    fan_state = false;
    light_state = false;
}

void fan_on(void) { fan_state = true; }
void fan_off(void) { fan_state = false; }
void light_on(void) { light_state = true; }
void light_off(void) { light_state = false; }
bool fan_is_on(void) { return fan_state; }
bool light_is_on(void) { return light_state; }

void control_reset(void)
{
    temperature_threshold = DEFAULT_TEMPERATURE_THRESHOLD_C;
    light_threshold = DEFAULT_LIGHT_THRESHOLD_LUX;
    current_mode = MODE_AUTOMATIC;
    relay_init();
}

void control_automatic(float temperature_c, float light_lux)
{
    if (current_mode != MODE_AUTOMATIC) {
        return;
    }
    if (!isnan(temperature_c)) {
        if (temperature_c > temperature_threshold) {
            fan_on();
        } else {
            fan_off();
        }
    }
    if (!isnan(light_lux)) {
        if (light_lux < light_threshold) {
            light_on();
        } else {
            light_off();
        }
    }
}

static bool equals_ignore_case(const char *left, const char *right)
{
    if (left == NULL || right == NULL) {
        return false;
    }
    while (*left != '\0' && *right != '\0') {
        if (tolower((unsigned char)*left) != tolower((unsigned char)*right)) {
            return false;
        }
        left++;
        right++;
    }
    return *left == '\0' && *right == '\0';
}

static bool extract_string(const char *json, const char *key, char *output,
                           size_t output_size)
{
    const char *position;
    const char *end;
    size_t length;
    char pattern[48];

    if (json == NULL || key == NULL || output == NULL || output_size == 0U) {
        return false;
    }
    if (strlen(key) + 3U >= sizeof(pattern)) {
        return false;
    }
    pattern[0] = '"';
    strcpy(pattern + 1, key);
    strcat(pattern, "\"");
    position = strstr(json, pattern);
    if (position == NULL) {
        return false;
    }
    position = strchr(position + strlen(pattern), ':');
    if (position == NULL) {
        return false;
    }
    position++;
    while (isspace((unsigned char)*position)) {
        position++;
    }
    if (*position != '"') {
        return false;
    }
    position++;
    end = strchr(position, '"');
    if (end == NULL) {
        return false;
    }
    length = (size_t)(end - position);
    if (length >= output_size) {
        return false;
    }
    memcpy(output, position, length);
    output[length] = '\0';
    return true;
}

static bool extract_float(const char *json, const char *key, float *output)
{
    char pattern[64];
    const char *position;
    char *end;

    if (json == NULL || key == NULL || output == NULL || strlen(key) + 3U >= sizeof(pattern)) {
        return false;
    }
    pattern[0] = '"';
    strcpy(pattern + 1, key);
    strcat(pattern, "\"");
    position = strstr(json, pattern);
    if (position == NULL) {
        return false;
    }
    position = strchr(position + strlen(pattern), ':');
    if (position == NULL) {
        return false;
    }
    position++;
    *output = strtof(position, &end);
    return end != position && isfinite(*output);
}

static control_status_t process_manual_control(const char *payload)
{
    char device[16];
    char command[16];

    if (current_mode != MODE_MANUAL) {
        return CONTROL_IGNORED;
    }
    if (!extract_string(payload, "device", device, sizeof(device)) ||
        !extract_string(payload, "command", command, sizeof(command))) {
        return CONTROL_INVALID_MESSAGE;
    }
    if (equals_ignore_case(device, "fan")) {
        if (equals_ignore_case(command, "ON")) {
            fan_on();
        } else if (equals_ignore_case(command, "OFF")) {
            fan_off();
        } else {
            return CONTROL_INVALID_MESSAGE;
        }
    } else if (equals_ignore_case(device, "light")) {
        if (equals_ignore_case(command, "ON")) {
            light_on();
        } else if (equals_ignore_case(command, "OFF")) {
            light_off();
        } else {
            return CONTROL_INVALID_MESSAGE;
        }
    } else {
        return CONTROL_INVALID_MESSAGE;
    }
    return CONTROL_OK;
}

static control_status_t process_thresholds(const char *payload)
{
    float value;
    bool changed = false;

    if (extract_float(payload, "nguong_nhiet_do", &value) && value >= -40.0F &&
        value <= 100.0F) {
        temperature_threshold = value;
        changed = true;
    }
    if (extract_float(payload, "nguong_anh_sang", &value) && value >= 0.0F) {
        light_threshold = value;
        changed = true;
    }
    return changed ? CONTROL_OK : CONTROL_INVALID_MESSAGE;
}

control_status_t control_process_message(const char *topic, const char *payload)
{
    if (topic == NULL || payload == NULL) {
        return CONTROL_INVALID_MESSAGE;
    }
    if (strcmp(topic, TOPIC_CONTROL) == 0) {
        return process_manual_control(payload);
    }
    if (strcmp(topic, TOPIC_THRESHOLD) == 0) {
        return process_thresholds(payload);
    }
    if (strcmp(topic, TOPIC_MODE) == 0) {
        if (equals_ignore_case(payload, "TU_DONG")) {
            current_mode = MODE_AUTOMATIC;
        } else if (equals_ignore_case(payload, "THU_CONG")) {
            current_mode = MODE_MANUAL;
        } else {
            return CONTROL_INVALID_MESSAGE;
        }
        return CONTROL_OK;
    }
    return CONTROL_UNKNOWN_TOPIC;
}

float control_temperature_threshold(void) { return temperature_threshold; }
float control_light_threshold(void) { return light_threshold; }
control_mode_t control_mode(void) { return current_mode; }
const char *control_mode_name(void)
{
    return current_mode == MODE_AUTOMATIC ? "TU_DONG" : "THU_CONG";
}
