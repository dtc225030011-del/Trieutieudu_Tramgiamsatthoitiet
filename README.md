# Trạm giám sát môi trường

Hệ thống IoT gồm một node ESP32 thu thập nhiệt độ, áp suất và cường độ ánh sáng; một MQTT broker làm lớp truyền thông; và ứng dụng Qt 6 chạy trên Linux/Raspberry Pi để giám sát, lưu lịch sử và điều khiển relay.

> README này mô tả theo mã nguồn hiện tại. Một số tài liệu trong `docs/` có thể dùng tên trường MQTT, vai trò hoặc schema CSDL của phiên bản cũ; khi có khác biệt, mã nguồn là nguồn tham chiếu ưu tiên.

## Kiến trúc

```text
BMP280 ─┐
        ├─ I2C (SDA GPIO21, SCL GPIO22) ─> ESP32 ─> MQTT Broker ─> Qt Host ─> SQLite
TSL2561 ┘                                      │
                                                ├─ GPIO26 ─> relay quạt
Qt Host ── MQTT lệnh/ngưỡng/chế độ ────────────┴─ GPIO27 ─> relay đèn
```

- **ESP32**: Arduino/PlatformIO; đọc cảm biến mỗi 2 giây, điều khiển relay, kết nối Wi-Fi và MQTT.
- **MQTT broker**: Mosquitto hoặc broker tương thích MQTT 3.1.1 qua TCP, mặc định cổng `1883`.
- **Qt Host**: C++17/Qt 6 Widgets; xác thực người dùng, dashboard, MQTT, SQLite, lịch sử và cảnh báo.

## Phần cứng

| Thiết bị | Kết nối | Chức năng |
| --- | --- | --- |
| BMP280 | I2C: SDA `GPIO21`, SCL `GPIO22` | Nhiệt độ và áp suất |
| TSL2561 | I2C: SDA `GPIO21`, SCL `GPIO22` | Cường độ ánh sáng (lux) |
| Relay quạt | `GPIO26` | Relay kích mức `HIGH` theo cấu hình firmware |
| Relay đèn | `GPIO27` | Relay kích mức `HIGH` theo cấu hình firmware |

BMP280 và TSL2561 dùng chung bus I2C. Firmware không gửi độ cao trong telemetry, dù BMP280 có thể hỗ trợ phép tính này.

## Chức năng đã hiện thực

- ESP32 đọc BMP280 và TSL2561 theo chu kỳ 2 giây, sau đó publish telemetry MQTT.
- Tự kết nối lại Wi-Fi (thử lại mỗi 10 giây) và MQTT (thử lại mỗi 5 giây).
- LWT MQTT cho trạng thái ESP32 `OFFLINE`, và publish `ONLINE` khi kết nối thành công.
- Hai chế độ điều khiển: `TU_DONG` và `THU_CONG`.
- Ở chế độ tự động, ESP32 bật quạt khi nhiệt độ lớn hơn ngưỡng và bật đèn khi ánh sáng nhỏ hơn ngưỡng.
- Ứng dụng Host nhận telemetry, hiển thị dữ liệu/trạng thái relay, lưu SQLite, hiển thị lịch sử, cảnh báo và lịch sử điều khiển.
- Host có lọc lịch sử dữ liệu theo khoảng thời gian.
- Đăng nhập, đăng xuất và quản lý tài khoản với hai vai trò: `ADMIN` và `USER`.
- Chỉ `ADMIN` có thể quản lý tài khoản, đổi chế độ, lưu ngưỡng và điều khiển relay thủ công.
- Có biểu đồ đường tự vẽ cho nhiệt độ, áp suất và ánh sáng; không dùng module Qt Charts.
- Ngăn mở nhiều phiên bản ứng dụng cùng lúc bằng lock file.

`csv_exporter.cpp` hiện có lớp xuất CSV UTF-8, nhưng chưa được gọi từ giao diện hoặc `AppController`; vì thế đây **chưa phải** là chức năng xuất CSV có thể thao tác từ ứng dụng hiện tại.

## Điều khiển tự động

Ngưỡng mặc định là `32.5 °C` và `55.0 lux`.

| Điều kiện ở `TU_DONG` | Kết quả |
| --- | --- |
| `nhiet_do > nguong_nhiet_do` | Bật quạt |
| `nhiet_do <= nguong_nhiet_do` | Tắt quạt |
| `anh_sang < nguong_anh_sang` | Bật đèn |
| `anh_sang >= nguong_anh_sang` | Tắt đèn |

Firmware chỉ nhận lệnh relay thủ công khi đang ở `THU_CONG`. Hệ thống chưa có hysteresis/vùng trễ, lịch biểu, PID hay cơ chế xác nhận lệnh riêng. Nếu cảm biến không đọc được dữ liệu, firmware không cập nhật relay tương ứng trong chu kỳ đó.

## MQTT API

### Kết nối

- MQTT v3.1.1 qua TCP.
- Broker mặc định phía Host: `127.0.0.1:1883`.
- Host và ESP32 có hỗ trợ username/password MQTT.
- ESP32 đặt keep-alive 30 giây và socket timeout 5 giây.

### Topics và payload thực tế

| Topic | Hướng | QoS / retain | Payload |
| --- | --- | --- | --- |
| `tramthoitiet/data` | ESP32 → Host | ESP32 publish QoS 0, không retain | Telemetry JSON |
| `tramthoitiet/status` | ESP32 → Host | QoS 1, retain | Chuỗi `ONLINE` hoặc `OFFLINE` |
| `tramthoitiet/control` | Host → ESP32 | QoS 1, không retain | JSON điều khiển relay |
| `tramthoitiet/threshold` | Host → ESP32 | QoS 1, retain | JSON ngưỡng |
| `tramthoitiet/mode` | Host → ESP32 | QoS 1, retain | Chuỗi chế độ |

Telemetry trên `tramthoitiet/data`:

```json
{
  "nhiet_do": 28.5,
  "ap_suat": 1013.25,
  "anh_sang": 120.0,
  "quat": 0,
  "den": 1,
  "nguong_nhiet_do": 32.5,
  "nguong_anh_sang": 55.0,
  "che_do": "TU_DONG"
}
```

Giá trị cảm biến không đọc được được ESP32 gửi là `null`. Qt Host gán thời gian nhận dữ liệu theo đồng hồ của máy Host; ESP32 không gửi timestamp/NTP trong payload.

Lệnh relay trên `tramthoitiet/control`:

```json
{"device":"fan","command":"ON"}
```

- `device`: `fan` hoặc `light`.
- `command`: `ON` hoặc `OFF`.
- Không hiện thực `state`, `all` hoặc `TOGGLE`.

Cập nhật ngưỡng trên `tramthoitiet/threshold`:

```json
{"nguong_nhiet_do":32.5,"nguong_anh_sang":55.0}
```

- Nhiệt độ hợp lệ: từ `-40` đến `100` °C.
- Ánh sáng hợp lệ: từ `0` lux trở lên.

Chuyển chế độ trên `tramthoitiet/mode` dùng payload text thuần:

```text
TU_DONG
```

hoặc:

```text
THU_CONG
```

## Cảnh báo và lịch sử

Host lưu dữ liệu và tạo cảnh báo cho nhiệt độ cao, ánh sáng thấp, mất/kết nối lại MQTT và ESP32 `OFFLINE`/`ONLINE` lại. Cảnh báo ngưỡng chỉ được ghi một lần khi trạng thái đang vi phạm; cờ cảnh báo được đặt lại khi dữ liệu trở về bình thường hoặc khi đổi ngưỡng.

Lịch sử điều khiển chỉ được ghi sau khi Host publish MQTT thành công. Bản ghi này không phải xác nhận ESP32 đã thực thi relay thành công; trạng thái sau cùng được phản ánh trong telemetry tiếp theo.

## Cơ sở dữ liệu

SQLite mặc định nằm ở `data/tram_thoi_tiet.sqlite`. Các bảng được tạo khi ứng dụng mở database:

| Bảng | Nội dung |
| --- | --- |
| `cau_hinh` | Cấu hình MQTT, ngưỡng và chế độ |
| `du_lieu_cam_bien` | Telemetry đã nhận |
| `lich_su_canh_bao` | Cảnh báo hệ thống và ngưỡng |
| `lich_su_dieu_khien` | Lệnh điều khiển/chế độ/ngưỡng kèm người dùng |
| `tai_khoan` | Người dùng, hash mật khẩu, vai trò và trạng thái kích hoạt |

Các bảng `sensor_history`, `alert_history` và `users` không phải schema hiện thực hiện tại.

## Phân quyền và cấu hình

- Vai trò hợp lệ trong database: `ADMIN`, `USER`.
- Mật khẩu tài khoản được băm SHA-256 trong mã nguồn hiện tại.
- Cấu hình dùng `QSettings` định dạng INI tại `config/cau_hinh.ini`:
  - `mqtt/host`, `mqtt/port`, `mqtt/username`, `mqtt/password`
  - `threshold/temperature`, `threshold/light`
  - `system/mode`
  - `database/path`

Không nên sử dụng hệ thống cho môi trường cần mức bảo mật cao khi chưa bổ sung TLS MQTT, bảo vệ file cấu hình chứa thông tin MQTT và cơ chế băm mật khẩu có salt/work factor.

## Cấu trúc mã nguồn

```text
esp32/
  src/main.cpp             # vòng lặp đọc cảm biến và gửi telemetry
  src/wifi_mqtt.cpp        # Wi-Fi, MQTT, LWT và publish telemetry
  src/xu_ly_lenh.cpp       # chế độ, ngưỡng và điều khiển relay
  src/bmp280.cpp           # driver BMP280
  src/tsl2561.cpp          # driver TSL2561
  src/relay.cpp            # điều khiển relay
qt/
  src/main.cpp             # khởi động, login và wiring signal/slot
  src/app_controller.cpp   # điều phối MQTT, database, cảnh báo
  src/mqtt_service.cpp     # libmosquitto
  src/database_service.cpp # SQLite và tài khoản
  src/cua_so_chinh.cpp     # giao diện, bảng lịch sử, biểu đồ tự vẽ
  src/settings_service.cpp # INI/QSettings
  src/csv_exporter.cpp     # lớp xuất CSV, chưa gắn vào UI
scripts/                   # build, chạy và triển khai Raspberry Pi
test_esp/                  # mã kiểm thử C độc lập cho một phần logic ESP32
```

## Yêu cầu môi trường

- CMake >= 3.18 và Ninja.
- Trình biên dịch hỗ trợ C++17.
- Qt 6: Widgets, Sql, Network, DBus.
- `libmosquitto` development package.
- Với ESP32: PlatformIO và các dependencies khai báo trong `esp32/platformio.ini`.
- Một MQTT broker đang chạy và thông tin Wi-Fi/MQTT phù hợp môi trường triển khai.

Ví dụ trên Ubuntu/Debian:

```bash
sudo apt update
sudo apt install -y build-essential cmake ninja-build pkg-config \
  qt6-base-dev qt6-tools-dev libmosquitto-dev mosquitto mosquitto-clients
```

## Build và chạy

### ESP32

1. Cập nhật cấu hình Wi-Fi và MQTT trong `esp32/include/cau_hinh.h` theo môi trường của bạn. Không commit thông tin xác thực thật.
2. Build và nạp firmware:

```bash
cd esp32
pio run -t upload
pio device monitor -b 115200
```

### Qt Host

Các script hiện tại có đường dẫn Qt/toolchain cục bộ được viết sẵn dưới `$HOME/Qt6Cross`; hãy điều chỉnh nếu môi trường của bạn khác.

```bash
./scripts/build_host.sh
./scripts/run_host.sh
```

Ứng dụng tạo/mở SQLite trước khi hiện hộp thoại đăng nhập. Tài khoản khởi tạo được tạo trong database khi chạy lần đầu; hãy đổi mật khẩu mặc định trước khi triển khai thực tế.

### Raspberry Pi ARM64

```bash
./scripts/build_arm64.sh
./scripts/deploy_pi.sh <IP_RASPBERRY_PI>
```

Script deploy sao lưu bản cài trước đó trên Raspberry Pi và giữ lại file INI cùng thư mục dữ liệu SQLite nếu chúng đã tồn tại.

## Giới hạn hiện tại

- MQTT đang dùng TCP thường, chưa có TLS.
- Telemetry ESP32 QoS 0 và không retain.
- Không có timestamp từ ESP32, OTA, REST API, ứng dụng web/mobile, cloud, AI/ML hoặc dự báo thời tiết.
- Không có xác nhận lệnh điều khiển riêng, cơ chế backup database hay HA cho broker.
- Không có hysteresis cho điều khiển relay.
- Lớp CSV có trong mã nhưng chưa tích hợp luồng thao tác của GUI.

## Kiểm thử

Thư mục `test_esp/` chứa bộ kiểm thử C độc lập với mock cho cảm biến, kết nối và điều khiển. Đây không phải kiểm thử trực tiếp firmware PlatformIO trên ESP32 thật.

```bash
cd test_esp
./run_tests.sh
```

## License

MIT. Xem [LICENSE](LICENSE).
