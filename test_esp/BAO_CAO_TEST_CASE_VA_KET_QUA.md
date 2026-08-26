# BÁO CÁO TEST CASE VÀ KẾT QUẢ

**Dự án:** Trạm Giám Sát Thời Tiết ESP32

**Công cụ:** Unity Test Framework / GCC host-side mock

**Tổng quan:** 41/41 test case PASS — `41 Tests 0 Failures 0 Ignored`

> Bộ test chạy độc lập bằng dữ liệu giả lập, không truy cập cảm biến, GPIO,
> Wi-Fi hoặc MQTT thật và không sửa mã nguồn firmware trong `esp32/`.

## Cảm biến nhiệt độ và áp suất BMP280

| Mã TC | Tên hàm test | Kết quả |
|:--:|---|:--:|
| BMP280-01 | `test_bmp280_init_success` | **PASS** |
| BMP280-02 | `test_bmp280_init_hardware_fault` | **PASS** |
| BMP280-03 | `test_bmp280_read_normal_measurement` | **PASS** |
| BMP280-04 | `test_bmp280_pressure_conversion_pa_to_hpa` | **PASS** |
| BMP280-05 | `test_bmp280_temperature_out_of_range` | **PASS** |
| BMP280-06 | `test_bmp280_pressure_out_of_range` | **PASS** |
| BMP280-07 | `test_bmp280_read_timeout` | **PASS** |
| BMP280-08 | `test_bmp280_null_pointer_safety` | **PASS** |

## Cảm biến cường độ ánh sáng TSL2561

| Mã TC | Tên hàm test | Kết quả |
|:--:|---|:--:|
| TSL2561-01 | `test_tsl2561_init_success` | **PASS** |
| TSL2561-02 | `test_tsl2561_init_hardware_fault` | **PASS** |
| TSL2561-03 | `test_tsl2561_read_normal_light` | **PASS** |
| TSL2561-04 | `test_tsl2561_negative_light_clamped_to_zero` | **PASS** |
| TSL2561-05 | `test_tsl2561_light_out_of_range` | **PASS** |
| TSL2561-06 | `test_tsl2561_read_timeout` | **PASS** |
| TSL2561-07 | `test_tsl2561_null_pointer_safety` | **PASS** |

## Relay, chế độ tự động/thủ công và lệnh MQTT

| Mã TC | Tên hàm test | Kết quả |
|:--:|---|:--:|
| CTRL-01 | `test_relay_init_safe_off` | **PASS** |
| CTRL-02 | `test_fan_manual_state_functions` | **PASS** |
| CTRL-03 | `test_light_manual_state_functions` | **PASS** |
| CTRL-04 | `test_automatic_hot_temperature_turns_fan_on` | **PASS** |
| CTRL-05 | `test_automatic_low_temperature_turns_fan_off` | **PASS** |
| CTRL-06 | `test_automatic_dark_environment_turns_light_on` | **PASS** |
| CTRL-07 | `test_automatic_bright_environment_turns_light_off` | **PASS** |
| CTRL-08 | `test_nan_sensor_value_preserves_relay_state` | **PASS** |
| CTRL-09 | `test_manual_mode_ignores_automatic_control` | **PASS** |
| CTRL-10 | `test_manual_fan_command` | **PASS** |
| CTRL-11 | `test_manual_light_command` | **PASS** |
| CTRL-12 | `test_control_command_ignored_in_automatic_mode` | **PASS** |
| CTRL-13 | `test_threshold_message_updates_valid_values` | **PASS** |
| CTRL-14 | `test_invalid_threshold_keeps_defaults` | **PASS** |
| CTRL-15 | `test_unknown_topic_is_rejected` | **PASS** |

## Kết nối Wi-Fi, MQTT, LWT, subscribe và telemetry

| Mã TC | Tên hàm test | Kết quả |
|:--:|---|:--:|
| NET-01 | `test_wifi_init_success` | **PASS** |
| NET-02 | `test_wifi_connect_success` | **PASS** |
| NET-03 | `test_wifi_authentication_failure` | **PASS** |
| NET-04 | `test_wifi_access_point_timeout` | **PASS** |
| NET-05 | `test_mqtt_requires_wifi_connection` | **PASS** |
| NET-06 | `test_mqtt_authentication_failure` | **PASS** |
| NET-07 | `test_mqtt_connect_subscribe_and_last_will` | **PASS** |
| NET-08 | `test_mqtt_publish_weather_telemetry` | **PASS** |
| NET-09 | `test_mqtt_publish_nan_as_json_null` | **PASS** |
| NET-10 | `test_mqtt_publish_while_disconnected` | **PASS** |
| NET-11 | `test_mqtt_auto_reconnect` | **PASS** |

## Kết luận

Bộ kiểm thử đã chạy 41 kịch bản cho luồng chuẩn, dữ liệu biên,
lỗi phần cứng, timeout, xác thực, mất kết nối và an toàn tham số.
Kết quả chi tiết của lần chạy gần nhất nằm trong `test_results.log`.
