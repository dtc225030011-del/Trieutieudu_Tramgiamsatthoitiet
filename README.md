# Trạm giám sát thời tiết

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
tram-thoi-tiet-host/
├── .github/
│   └── workflows/
│       ├── code-quality.yml     # Workflow kiểm tra định dạng và chất lượng mã nguồn
│       ├── esp32-ci.yml         # Workflow CI biên dịch firmware PlatformIO
│       └── qt-ci.yml            # Workflow CI biên dịch ứng dụng Qt 6 Host
├── docs/                        # Tài liệu đặc tả kỹ thuật chi tiết
│   ├── ARCHITECTURE.md          # Kiến trúc tổng thể và luồng truyền dữ liệu
│   ├── DEPLOYMENT_GUIDE.md      # Hướng dẫn biên dịch và triển khai thực tế
│   ├── HARDWARE_PINOUT.md       # Sơ đồ kết nối chân GPIO và phần cứng cảm biến/relay
│   └── MQTT_API.md              # Quy chuẩn định dạng gói tin MQTT và topics
├── esp32/                       # Mã nguồn firmware vi điều khiển ESP32 (PlatformIO)
│   ├── include/
│   │   ├── bmp280.h             # Khai báo driver cảm biến nhiệt độ & áp suất BMP280
│   │   ├── cau_hinh.example.h   # Mẫu cấu hình Wi-Fi, MQTT broker và chân GPIO
│   │   ├── cau_hinh.h           # File cấu hình hoạt động thực tế của firmware
│   │   ├── relay.h              # Khai báo hàm điều khiển relay quạt và đèn
│   │   ├── tsl2561.h            # Khai báo driver cảm biến cường độ ánh sáng TSL2561
│   │   ├── wifi_mqtt.h          # Quản lý kết nối mạng, MQTT client và publish telemetry
│   │   └── xu_ly_lenh.h         # Xử lý lệnh nhận từ MQTT và thuật toán điều khiển tự động
│   ├── src/
│   │   ├── bmp280.cpp           # Hiện thực giao tiếp I2C với cảm biến BMP280
│   │   ├── main.cpp             # Điểm khởi tạo và vòng lặp chính (đọc cảm biến 2s/lần)
│   │   ├── relay.cpp            # Hiện thực kích mức logic điều khiển đóng/ngắt relay
│   │   ├── tsl2561.cpp          # Hiện thực cấu hình độ lợi và đọc lux từ TSL2561
│   │   ├── wifi_mqtt.cpp        # Hiện thực kết nối Wi-Fi/MQTT, cơ chế LWT và tự kết nối lại
│   │   └── xu_ly_lenh.cpp       # Phân tích payload JSON lệnh/ngưỡng/chế độ và thực thi
│   └── platformio.ini           # Cấu hình môi trường build PlatformIO và thư viện phụ thuộc
├── package/                     # Cấu trúc thư mục gói triển khai thực thi trên Raspberry Pi
├── qt/                          # Ứng dụng máy chủ giám sát & điều khiển giao diện Qt 6
│   ├── CMakeLists.txt           # Cấu hình biên dịch CMake (C++17, Qt6, libmosquitto)
│   ├── include/
│   │   ├── app_controller.h     # Bộ điều phối trung tâm giữa UI, MQTT, Database và Settings
│   │   ├── canh_bao_data.h      # Khai báo cấu trúc dữ liệu lưu trữ thông tin cảnh báo
│   │   ├── csv_exporter.h       # Tiện ích xuất dữ liệu cảm biến ra file định dạng CSV
│   │   ├── cua_so_chinh.h       # Lớp điều khiển cửa sổ chính MainWindow
│   │   ├── dang_nhap.h          # Lớp điều khiển hộp thoại đăng nhập xác thực người dùng
│   │   ├── database_service.h   # Dịch vụ quản trị cơ sở dữ liệu SQLite cục bộ
│   │   ├── mqtt_service.h       # Dịch vụ giao tiếp mạng MQTT thông qua libmosquitto
│   │   ├── quan_ly_tai_khoan.h  # Lớp điều khiển hộp thoại phân quyền & quản lý tài khoản
│   │   ├── sensor_data.h        # Khai báo cấu trúc dữ liệu gói tin telemetry
│   │   └── settings_service.h   # Dịch vụ lưu trữ và nạp cấu hình hệ thống từ file INI
│   ├── resources/
│   │   └── resources.qrc        # Định nghĩa các tài nguyên nhúng (icon, giao diện)
│   ├── src/
│   │   ├── app_controller.cpp   # Xử lý logic nghiệp vụ, phân tích cảnh báo, cầu nối UI - dịch vụ
│   │   ├── csv_exporter.cpp     # Hiện thực ghi dữ liệu ra định dạng CSV UTF-8
│   │   ├── cua_so_chinh.cpp     # Giao diện dashboard, vẽ biểu đồ thời gian thực, bảng lịch sử
│   │   ├── dang_nhap.cpp        # Xác thực tài khoản với CSDL bằng mã băm SHA-256
│   │   ├── database_service.cpp # Khởi tạo bảng CSDL, lưu telemetry, log cảnh báo và điều khiển
│   │   ├── main.cpp             # Điểm bắt đầu ứng dụng, kiểm tra lock file đơn phiên bản
│   │   ├── mqtt_service.cpp     # Kết nối broker, quản lý luồng nhận tin và bắn Qt signal
│   │   ├── quan_ly_tai_khoan.cpp# Giao diện quản trị viên thêm/sửa/xóa/vô hiệu hóa tài khoản
│   │   └── settings_service.cpp # Đọc/ghi thiết lập ngưỡng, thông số broker vào file INI
│   └── ui/                      # File thiết kế giao diện đồ họa (Qt Designer)
│       ├── cua_so_chinh.ui       # Thiết kế bố cục màn hình làm việc chính
│       ├── dang_nhap.ui         # Thiết kế bố cục hộp thoại đăng nhập
│       └── quan_ly_tai_khoan.ui # Thiết kế bố cục hộp thoại quản lý người dùng
├── scripts/                     # Tập hợp shell script hỗ trợ build và triển khai
│   ├── build_arm64.sh           # Script biên dịch gói chạy cho Raspberry Pi (ARM64)
│   ├── build_host.sh            # Script cấu hình CMake và biên dịch Qt Host trên máy tính
│   ├── deploy_pi.sh             # Script đóng gói và đồng bộ ứng dụng lên Raspberry Pi qua SSH
│   ├── run_from_qtcreator.sh    # Script cấu hình môi trường khi debug/run từ Qt Creator
│   └── run_host.sh              # Script thiết lập thư viện động và khởi chạy ứng dụng Host
└── test_esp/                    # Bộ kiểm thử Unit Test C độc lập cho firmware ESP32
    ├── Makefile                 # Makefile biên dịch bộ kiểm thử bằng GCC
    ├── run_tests.sh             # Script thực thi toàn bộ test case và tổng hợp log
    ├── generate_report.py       # Script sinh báo cáo kết quả kiểm thử tự động (HTML/Markdown)
    ├── BAO_CAO_TEST_CASE_VA_KET_QUA.md # Báo cáo chi tiết 19/19 test cases kiểm thử
    ├── unity/                   # Thư viện kiểm thử nhúng Unity Test Framework (C)
    │   ├── unity.c
    │   ├── unity.h
    │   └── unity_internals.h
    ├── include/                 # Khai báo interface mock cho kiểm thử
    │   ├── esp_connectivity.h   # Mock kết nối Wi-Fi, MQTT state và LWT
    │   ├── esp_control.h        # Mock logic điều khiển tự động và thủ công
    │   └── esp_sensors.h        # Mock giao tiếp cảm biến BMP280, TSL2561
    ├── src/                     # Hiện thực mock logic kiểm thử
    │   ├── esp_connectivity.c   # Logic giả lập trạng thái mạng và publish
    │   ├── esp_control.c        # Logic giả lập thuật toán kích relay theo ngưỡng
    │   └── esp_sensors.c        # Logic giả lập đọc giá trị cảm biến và xử lý lỗi
    └── test/                    # Các kịch bản kiểm thử tự động
        ├── test_bmp280.c        # Kiểm thử đọc BMP280, dải đo, timeout và lỗi bus I2C
        ├── test_connectivity.c  # Kiểm thử kết nối lại Wi-Fi/MQTT, publish và LWT
        ├── test_control.c       # Kiểm thử điều khiển relay tự động và thủ công
        ├── test_main.c          # Entry point tổng hợp chạy toàn bộ test runner
        └── test_tsl2561.c       # Kiểm thử đọc lux TSL2561, độ lợi và giá trị biên
```

### Chi tiết các khối thành phần

1. **Firmware ESP32 (`esp32/`)**:
   - Sử dụng PlatformIO với framework Arduino cho vi điều khiển ESP32.
   - Quản lý đọc dữ liệu đồng thời từ 2 cảm biến trên cùng bus I2C (`BMP280` và `TSL2561`) theo chu kỳ định kỳ 2 giây.
   - Tự động duy trì kết nối Wi-Fi/MQTT, hỗ trợ LWT (`tramthoitiet/status`) và gửi telemetry JSON (`tramthoitiet/data`).
   - Hỗ trợ 2 chế độ điều khiển: Tự động (so khớp nhiệt độ/ánh sáng với ngưỡng cấu hình) và Thủ công (thực thi lệnh từ topic `tramthoitiet/control`).

2. **Ứng dụng giám sát Qt Host (`qt/`)**:
   - Xây dựng trên C++17 và Qt 6 (Widgets, Sql, Network, DBus).
   - Kiến trúc module hóa theo mô hình phân tầng:
     - `AppController`: Bộ điều phối trung tâm xử lý dữ liệu telemetry, phát hiện vi phạm ngưỡng, sinh cảnh báo và xử lý lệnh điều khiển.
     - `MqttService`: Đóng gói thư viện `libmosquitto`, duy trì luồng giao tiếp mạng riêng biệt và đồng bộ với giao diện qua Qt Signal/Slot.
     - `DatabaseService`: Tương tác với cơ sở dữ liệu SQLite cục bộ, lưu trữ toàn bộ lịch sử đo, nhật ký cảnh báo, lịch sử tác vụ điều khiển và thông tin tài khoản.
     - `SettingsService`: Quản lý các thiết lập runtime (thông tin broker, ngưỡng cảnh báo, chế độ hoạt động) thông qua file INI.
     - `MainWindow` / UI: Trực quan hóa dữ liệu tức thời, vẽ biểu đồ đường theo thời gian thực (custom render với `QPainter`), bảng lịch sử lọc theo mốc thời gian, quản lý danh sách cảnh báo và phân quyền người dùng (`ADMIN` / `USER`).

3. **Bộ kiểm thử độc lập (`test_esp/`)**:
   - Xây dựng bằng ngôn ngữ C thuần tích hợp framework **Unity Test**, độc lập hoàn toàn với phần cứng ESP32 thực tế để có thể chạy kiểm thử tức thì trên môi trường Host và CI.
   - Bao phủ 19 test cases cho 4 module nghiệp vụ chính: cảm biến BMP280, cảm biến TSL2561, thuật toán điều khiển relay và quản lý trạng thái kết nối Wi-Fi/MQTT.
   - Đi kèm script sinh báo cáo kết quả tự động định dạng HTML và Markdown trực quan (`BAO_CAO_TEST_CASE_VA_KET_QUA.md`).

4. **Kịch bản tự động hóa (`scripts/` & `.github/workflows/`)**:
   - Cung cấp đầy đủ shell script cho quy trình phát triển: biên dịch host (`build_host.sh`), chạy ứng dụng (`run_host.sh`), cross-compile ARM64 (`build_arm64.sh`) và deploy tự động qua mạng lên Raspberry Pi (`deploy_pi.sh`).
   - Thiết lập GitHub Actions CI tự động kiểm tra định dạng code, biên dịch firmware ESP32 và build kiểm thử ứng dụng Qt Host khi có commit/PR mới.

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
