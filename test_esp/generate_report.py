#!/usr/bin/env python3
"""Generate Markdown and HTML summaries from the latest Unity console log."""

from __future__ import annotations

import html
import re
from pathlib import Path


ROOT = Path(__file__).resolve().parent
LOG_PATH = ROOT / "test_results.log"
MARKDOWN_PATH = ROOT / "BAO_CAO_TEST_CASE_VA_KET_QUA.md"
HTML_PATH = ROOT / "BAO_CAO_TEST_CASE_VA_KET_QUA.html"

GROUPS = [
    (
        "BMP280",
        "Cảm biến nhiệt độ và áp suất BMP280",
        [
            "test_bmp280_init_success",
            "test_bmp280_init_hardware_fault",
            "test_bmp280_read_normal_measurement",
            "test_bmp280_pressure_conversion_pa_to_hpa",
            "test_bmp280_temperature_out_of_range",
            "test_bmp280_pressure_out_of_range",
            "test_bmp280_read_timeout",
            "test_bmp280_null_pointer_safety",
        ],
    ),
    (
        "TSL2561",
        "Cảm biến cường độ ánh sáng TSL2561",
        [
            "test_tsl2561_init_success",
            "test_tsl2561_init_hardware_fault",
            "test_tsl2561_read_normal_light",
            "test_tsl2561_negative_light_clamped_to_zero",
            "test_tsl2561_light_out_of_range",
            "test_tsl2561_read_timeout",
            "test_tsl2561_null_pointer_safety",
        ],
    ),
    (
        "CTRL",
        "Relay, chế độ tự động/thủ công và lệnh MQTT",
        [
            "test_relay_init_safe_off",
            "test_fan_manual_state_functions",
            "test_light_manual_state_functions",
            "test_automatic_hot_temperature_turns_fan_on",
            "test_automatic_low_temperature_turns_fan_off",
            "test_automatic_dark_environment_turns_light_on",
            "test_automatic_bright_environment_turns_light_off",
            "test_nan_sensor_value_preserves_relay_state",
            "test_manual_mode_ignores_automatic_control",
            "test_manual_fan_command",
            "test_manual_light_command",
            "test_control_command_ignored_in_automatic_mode",
            "test_threshold_message_updates_valid_values",
            "test_invalid_threshold_keeps_defaults",
            "test_unknown_topic_is_rejected",
        ],
    ),
    (
        "NET",
        "Kết nối Wi-Fi, MQTT, LWT, subscribe và telemetry",
        [
            "test_wifi_init_success",
            "test_wifi_connect_success",
            "test_wifi_authentication_failure",
            "test_wifi_access_point_timeout",
            "test_mqtt_requires_wifi_connection",
            "test_mqtt_authentication_failure",
            "test_mqtt_connect_subscribe_and_last_will",
            "test_mqtt_publish_weather_telemetry",
            "test_mqtt_publish_nan_as_json_null",
            "test_mqtt_publish_while_disconnected",
            "test_mqtt_auto_reconnect",
        ],
    ),
]


def read_results() -> tuple[dict[str, str], str]:
    if not LOG_PATH.exists():
        raise SystemExit("Chưa có test_results.log. Hãy chạy ./run_tests.sh trước.")
    log = LOG_PATH.read_text(encoding="utf-8")
    results: dict[str, str] = {}
    for name, status in re.findall(r":(test_[A-Za-z0-9_]+):(PASS|FAIL|IGNORE)", log):
        results[name] = status
    match = re.search(r"(\d+ Tests \d+ Failures \d+ Ignored)", log)
    summary = match.group(1) if match else "Không đọc được tổng kết Unity"
    return results, summary


def markdown_report(results: dict[str, str], summary: str) -> str:
    passed = sum(status == "PASS" for status in results.values())
    total = sum(len(cases) for _, _, cases in GROUPS)
    lines = [
        "# BÁO CÁO TEST CASE VÀ KẾT QUẢ",
        "",
        "**Dự án:** Trạm Giám Sát Thời Tiết ESP32",
        "",
        "**Công cụ:** Unity Test Framework / GCC host-side mock",
        "",
        f"**Tổng quan:** {passed}/{total} test case PASS — `{summary}`",
        "",
        "> Bộ test chạy độc lập bằng dữ liệu giả lập, không truy cập cảm biến, GPIO,",
        "> Wi-Fi hoặc MQTT thật và không sửa mã nguồn firmware trong `esp32/`.",
        "",
    ]
    for prefix, title, cases in GROUPS:
        lines.extend(
            [
                f"## {title}",
                "",
                "| Mã TC | Tên hàm test | Kết quả |",
                "|:--:|---|:--:|",
            ]
        )
        for index, name in enumerate(cases, 1):
            status = results.get(name, "NOT RUN")
            lines.append(f"| {prefix}-{index:02d} | `{name}` | **{status}** |")
        lines.append("")
    lines.extend(
        [
            "## Kết luận",
            "",
            f"Bộ kiểm thử đã chạy {total} kịch bản cho luồng chuẩn, dữ liệu biên,",
            "lỗi phần cứng, timeout, xác thực, mất kết nối và an toàn tham số.",
            "Kết quả chi tiết của lần chạy gần nhất nằm trong `test_results.log`.",
            "",
        ]
    )
    return "\n".join(lines)


def html_report(results: dict[str, str], summary: str) -> str:
    passed = sum(status == "PASS" for status in results.values())
    total = sum(len(cases) for _, _, cases in GROUPS)
    sections = []
    for prefix, title, cases in GROUPS:
        rows = []
        for index, name in enumerate(cases, 1):
            status = results.get(name, "NOT RUN")
            css_class = "pass" if status == "PASS" else "fail"
            rows.append(
                f"<tr><td>{prefix}-{index:02d}</td>"
                f"<td><code>{html.escape(name)}</code></td>"
                f'<td class="{css_class}">{status}</td></tr>'
            )
        sections.append(
            f"<h2>{html.escape(title)}</h2><table><thead><tr>"
            "<th>Mã TC</th><th>Tên hàm test</th><th>Kết quả</th>"
            f"</tr></thead><tbody>{''.join(rows)}</tbody></table>"
        )
    return f"""<!doctype html>
<html lang="vi"><head><meta charset="utf-8">
<meta name="viewport" content="width=device-width,initial-scale=1">
<title>Báo cáo kiểm thử ESP32 - Trạm thời tiết</title>
<style>
body{{font:16px system-ui,sans-serif;max-width:1100px;margin:40px auto;padding:0 20px;color:#172033}}
h1{{color:#174ea6}} h2{{margin-top:32px;color:#2459a9}}
.summary{{padding:16px;background:#eef5ff;border-left:5px solid #2563eb}}
table{{border-collapse:collapse;width:100%;margin:12px 0 24px}}
th,td{{border:1px solid #d8dee9;padding:9px;text-align:left}}
th{{background:#eaf0f8}} .pass{{color:#157f3b;font-weight:700}}
.fail{{color:#b42318;font-weight:700}} code{{font-size:14px}}
</style></head><body>
<h1>BÁO CÁO TEST CASE VÀ KẾT QUẢ</h1>
<p><strong>Dự án:</strong> Trạm Giám Sát Thời Tiết ESP32</p>
<p><strong>Công cụ:</strong> Unity Test Framework / GCC host-side mock</p>
<p class="summary"><strong>{passed}/{total} test case PASS</strong> — {html.escape(summary)}</p>
{''.join(sections)}
<h2>Kết luận</h2>
<p>Bộ kiểm thử chạy độc lập bằng dữ liệu giả lập, không truy cập phần cứng thật và
không sửa mã nguồn firmware trong thư mục <code>esp32/</code>.</p>
</body></html>
"""


def main() -> None:
    results, summary = read_results()
    MARKDOWN_PATH.write_text(markdown_report(results, summary), encoding="utf-8")
    HTML_PATH.write_text(html_report(results, summary), encoding="utf-8")
    print(f"Đã tạo {MARKDOWN_PATH.name} và {HTML_PATH.name}")


if __name__ == "__main__":
    main()
