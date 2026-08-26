# KIỂM THỬ UNITY CHO ESP32 — TRẠM GIÁM SÁT THỜI TIẾT

Thư mục này là bộ kiểm thử host-side độc lập, được xây dựng theo cấu trúc của
repo mẫu. Dữ liệu cảm biến, GPIO, Wi-Fi và MQTT đều được mock/stub nên chạy test
không cần bo ESP32, không cần broker và không thay đổi mã nguồn trong `esp32/`.

## Cấu trúc

```text
test_esp/
├── unity/                 # Unity Test Framework tối giản dùng cho bộ test
├── include/               # API cảm biến, điều khiển và kết nối giả lập
├── src/                   # Mock/stub cùng logic đặc tả cần kiểm thử
├── test/                  # 41 test case chia thành 4 nhóm
├── Makefile               # Build bằng GCC
├── run_tests.sh           # Build, chạy test và tạo báo cáo
├── generate_report.py     # Sinh báo cáo Markdown và HTML từ log Unity
├── BAO_CAO_TEST_CASE_VA_KET_QUA.md
└── BAO_CAO_TEST_CASE_VA_KET_QUA.html
```

## Chạy test

Từ thư mục gốc dự án:

```bash
cd test_esp
chmod +x run_tests.sh
./run_tests.sh
```

Hoặc dùng Make trực tiếp:

```bash
cd test_esp
make clean
make test
python3 generate_report.py
```

Khi thành công, dòng tổng kết là:

```text
41 Tests 0 Failures 0 Ignored
OK - ALL TESTS PASSED!
```

## Phạm vi 41 test case

| Nhóm | Số lượng | Nội dung |
|---|---:|---|
| BMP280 | 8 | Khởi tạo, đọc nhiệt độ/áp suất, đổi Pa → hPa, dải đo, timeout, NULL |
| TSL2561 | 7 | Khởi tạo, đọc lux, chặn số âm, dải đo, timeout, NULL |
| Relay và điều khiển | 15 | Trạng thái an toàn, AUTO/MANUAL, NaN, JSON lệnh và cập nhật ngưỡng |
| Wi-Fi và MQTT | 11 | Xác thực, timeout, LWT, subscribe, telemetry, JSON null, reconnect |

## Lưu ý về mức độ kiểm thử

Đây là unit test bằng mô phỏng theo cách làm của repo mẫu. Nó kiểm tra đặc tả
logic và các tình huống lỗi trên máy host; không thay thế kiểm thử tích hợp với
thư viện Arduino/Adafruit/PubSubClient hoặc kiểm thử trên phần cứng thật.
