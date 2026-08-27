# 🌦️ Trạm Theo Dõi

[![Qt Version](https://img.shields.io/badge/Qt-6.x%20%7C%20C%2B%2B17-41CD52?logo=qt&logoColor=white)](https://www.qt.io/)
[![PlatformIO](https://img.shields.io/badge/PlatformIO-ESP32%20Arduino-f38b00?logo=platformio&logoColor=white)](https://platformio.org/)
[![Protocol](https://img.shields.io/badge/Protocol-MQTT%20%2F%20JSON-0083C0?logo=mqtt&logoColor=white)](https://mqtt.org/)
[![Database](https://img.shields.io/badge/Database-SQLite3-003B57?logo=sqlite&logoColor=white)](https://www.sqlite.org/)
[![Target](https://img.shields.io/badge/Target-Linux%20Host%20%2F%20Raspberry%20Pi-C51A4A?logo=raspberrypi&logoColor=white)](https://www.raspberrypi.com/)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)
[![Build Status](https://github.com/blynkapp04-ui/tram-thoi-tiet/actions/workflows/qt-ci.yml/badge.svg)](https://github.com/blynkapp04-ui/tram-thoi-tiet/actions)

---

## 📌 Giới thiệu (Overview)

**Trạm Quan Trắc Thời Tiết** là hệ thống giám sát môi trường và điều khiển tự động toàn diện chuẩn công nghiệp IoT, kết hợp giữa:
1. **Node cảm biến phần cứng (ESP32 Firmware)**: Thu thập dữ liệu cảm biến áp suất, nhiệt độ, độ cao ([BMP280](https://www.bosch-sensortec.com/)) và cường độ ánh sáng ([TSL2561](https://ams.com/)), đóng cắt rơ-le thông minh và truyền dữ liệu qua giao thức MQTT.
2. **Ứng dụng máy chủ giám sát (Qt6 Host / Raspberry Pi Dashboard GUI)**: Giao diện trực quan hiện đại viết bằng C++17 & Qt6 Widgets, tích hợp xác thực phân quyền tài khoản (RBAC), quản lý cấu hình INI, cơ sở dữ liệu SQLite3, xuất báo cáo CSV, vẽ biểu đồ thời gian thực và cảnh báo ngưỡng tự động.

---

## 🏗️ Kiến trúc Hệ thống (System Architecture)

```
 +-------------------------------------------------------------------------+
 |                            HARDWARE LAYER                               |
 |                                                                         |
 |  +--------------------+    I2C (SDA:21, SCL:22)     +----------------+  |
 |  | BMP280 Sensor      | -------------------------> |                |  |
 |  | (Temp, Pressure)   |                            |                |  |
 |  +--------------------+                            |  ESP32 MCU     |  |
 |                                                    | (FreeRTOS /    |  |
 |  +--------------------+    I2C (SDA:21, SCL:22)    |  Arduino Core) |  |
 |  | TSL2561 Sensor     | -------------------------> |                |  |
 |  | (Light Lux)        |                            |                |  |
 |  +--------------------+                            |                |  |
 |                                                    |                |  |
 |  +--------------------+    GPIO 26 (Fan), 27 (Lamp)|                |  |
 |  | Relay Actuators    | <------------------------- |                |  |
 |  +--------------------+                            +--------+-------+  |
 +-------------------------------------------------------------|-----------+
                                                               | Wi-Fi (WPA2)
                                                               | MQTT (TCP/1883)
                                                               v
 +-------------------------------------------------------------------------+
 |                       COMMUNICATION & BROKER                            |
 |                                                                         |
 |                      Eclipse Mosquitto MQTT Broker                      |
 |                 Topics: data, status, control, threshold, mode          |
 +-------------------------------------------------------------|-----------+
                                                               |
                                                               v
 +-------------------------------------------------------------------------+
 |                         HOST / DASHBOARD LAYER                          |
 |                                                                         |
 |  +-------------------------------------------------------------------+  |
 |  | Qt6 Host C++ Application (Desktop Linux x86_64 / Raspberry Pi ARM64) |
 |  |                                                                   |  |
 |  |  - GUI Dashboard (Real-time telemetry, Charts, Status Indicator)  |  |
 |  |  - Auth & RBAC (Admin, Operator, User management)                 |  |
 |  |  - SQLite3 Engine (History logging, Search, Indexing)             |  |
 |  |  - CSV Exporter (Export reports with customizable range)          |  |
 |  |  - Threshold Controller & Actuator Management (Auto / Manual)     |  |
 |  +-------------------------------------------------------------------+  |
 +-------------------------------------------------------------------------+
```

---

## ✨ Tính năng Nổi bật (Features)

- ⚡ **Thu thập dữ liệu thời gian thực (Real-time Telemetry)**: Tần số lấy mẫu 2s/lần cho nhiệt độ, áp suất khí quyển, độ cao ước tính và cường độ ánh sáng (Lux).
- 🛡️ **Quản trị người dùng & Phân quyền (RBAC)**: Đăng nhập bảo mật, quản lý tài khoản theo vai trò (Admin / Quản lý / Nhân viên).
- 🎛️ **Chế độ điều khiển kép (Dual Mode Control)**:
  - **Tự động (Auto)**: Tự động kích hoạt quạt khi nhiệt độ vượt ngưỡng, bật đèn khi trời tối.
  - **Thủ công (Manual)**: Điều khiển bật/tắt thiết bị tức thì từ màn hình Host GUI hoặc qua MQTT.
- 💾 **Lưu trữ & Xuất báo cáo (Database & Export)**: Ghi log liên tục vào cơ sở dữ liệu SQLite3, xuất dữ liệu lịch sử ra định dạng CSV phục vụ phân tích.
- 🚀 **Hỗ trợ đa nền tảng (Cross-Platform)**: Chạy mượt mà trên máy tính Host Linux (x86_64) và bo mạch nhúng Raspberry Pi 3/4/5 (ARM64).
- 🔄 **Khả năng tự phục hồi (Auto-reconnect & Fail-safe)**: Tự động kết nối lại Wi-Fi và MQTT Broker khi mất mạng, đảm bảo trạng thái an toàn cho rơ-le.

---

## 📁 Cấu trúc Thư mục (Directory Structure)

```
tram-thoi-tiet/
├── .github/                              # Cấu hình GitHub Actions CI/CD & biểu mẫu
│   ├── ISSUE_TEMPLATE/                   # Mẫu báo cáo issue
│   │   ├── bug_report.md                 # Mẫu báo cáo lỗi phần mềm / phần cứng
│   │   └── feature_request.md            # Mẫu đề xuất phát triển tính năng mới
│   ├── PULL_REQUEST_TEMPLATE.md          # Biểu mẫu chuẩn khi mở Pull Request
│   └── workflows/                        # Quy trình kiểm thử & tự động hóa CI/CD
│       ├── code-quality.yml              # Kiểm tra quy chuẩn mã nguồn C++ (Clang-Format)
│       ├── esp32-ci.yml                  # Tự động build & kiểm tra firmware ESP32
│       └── qt-ci.yml                     # Tự động build ứng dụng Qt6 Host trên Ubuntu runner
├── docs/                                 # Tài liệu kỹ thuật chi tiết của dự án
│   ├── ARCHITECTURE.md                   # Kiến trúc phần mềm, sơ đồ luồng dữ liệu & State Machine
│   ├── DEPLOYMENT_GUIDE.md               # Hướng dẫn chi tiết triển khai máy tính Host & Raspberry Pi
│   ├── HARDWARE_PINOUT.md                # Sơ đồ nối dây GPIO/I2C, thông số cảm biến & rơ-le
│   └── MQTT_API.md                       # Đặc tả chi tiết MQTT Topics, JSON schema & chu trình bản tin
├── esp32/                                # Mã nguồn Firmware ESP32 (PlatformIO)
│   ├── include/                          # Header files cấu hình & driver phần cứng
│   │   ├── bmp280.h                      # Driver cảm biến nhiệt độ & áp suất BMP280 (I2C)
│   │   ├── cau_hinh.h                    # Cấu hình Wi-Fi, MQTT Broker, chân GPIO
│   │   ├── cau_hinh.example.h            # Cấu hình mẫu ban đầu
│   │   ├── relay.h                       # Module điều khiển rơ-le Quạt & Đèn
│   │   ├── tsl2561.h                     # Driver cảm biến cường độ ánh sáng TSL2561 (I2C)
│   │   ├── wifi_mqtt.h                   # Quản lý kết nối Wi-Fi & MQTT Client
│   │   └── xu_ly_lenh.h                  # Xử lý gói tin JSON, cảnh báo ngưỡng & điều khiển
│   ├── src/                              # Mã nguồn triển khai logic Firmware C++
│   │   ├── bmp280.cpp                    # Hiện thực khởi tạo & đọc dữ liệu BMP280
│   │   ├── main.cpp                      # Hàm setup(), loop() chính và chu kỳ lấy mẫu 2s
│   │   ├── relay.cpp                     # Hiện thực điều khiển trạng thái GPIO rơ-le
│   │   ├── tsl2561.cpp                   # Hiện thực đọc cường độ sáng Lux từ TSL2561
│   │   ├── wifi_mqtt.cpp                 # Hiện thực kết nối, auto-reconnect Wi-Fi/MQTT & gửi telemetry
│   │   └── xu_ly_lenh.cpp                # Hiện thực logic tự động/thủ công & thực thi lệnh
│   └── platformio.ini                    # File cấu hình PlatformIO (board, framework, dependencies)
├── package/                              # Đóng gói sản phẩm để phân phối lên Raspberry Pi
│   ├── tram-thoi-tiet/                   # Thư mục gốc chứa gói ứng dụng chạy độc lập
│   │   ├── bin/                          # Thư mục chứa file nhị phân thực thi ứng dụng
│   │   ├── config/                       # Thư mục cấu hình hệ thống
│   │   │   └── cau_hinh.ini              # File INI cấu hình Broker, Database, Threshold mặc định
│   │   ├── data/                         # Thư mục chứa cơ sở dữ liệu SQLite (tram_thoi_tiet.db)
│   │   ├── logs/                         # Thư mục lưu trữ file log hoạt động của ứng dụng
│   │   └── scripts/                      # Thư mục kịch bản vận hành
│   │       └── run_pi.sh                 # Script nạp môi trường và khởi chạy trên Raspberry Pi
│   └── tram-thoi-tiet-arm64.tar.gz       # Gói nén lưu trữ sẵn sàng phân phối cho ARM64
├── qt/                                   # Mã nguồn Ứng dụng Qt6 C++ Host GUI Dashboard
│   ├── CMakeLists.txt                    # Kịch bản biên dịch CMake cho ứng dụng Qt6
│   ├── include/                          # Header files định nghĩa các lớp và cấu trúc dữ liệu
│   │   ├── app_controller.h              # Central Controller điều phối kết nối UI, Service và State
│   │   ├── canh_bao_data.h               # Cấu trúc dữ liệu sự kiện cảnh báo ngưỡng (Alert Struct)
│   │   ├── csv_exporter.h                # Module xuất dữ liệu lịch sử đo đạc ra file CSV
│   │   ├── cua_so_chinh.h                # Quản lý giao diện chính MainWindow, biểu đồ & bảng dữ liệu
│   │   ├── dang_nhap.h                   # Dialog đăng nhập và xác thực người dùng
│   │   ├── database_service.h            # SQLite3 Database Service (quản lý telemetry, user, alert log)
│   │   ├── mqtt_service.h                # MQTT Client Service (kết nối, subscribe/publish dữ liệu)
│   │   ├── quan_ly_tai_khoan.h           # Form & logic quản trị người dùng, phân quyền RBAC
│   │   ├── sensor_data.h                 # Cấu trúc dữ liệu gói tin cảm biến (Sensor Data Struct)
│   │   └── settings_service.h            # Service đọc/ghi file cấu hình INI (QSettings)
│   ├── resources/                        # Tài nguyên đồ họa nhúng ứng dụng
│   │   └── resources.qrc                 # File định nghĩa Qt Resource Collection (icons, styles)
│   ├── src/                              # Mã nguồn triển khai logic C++ Qt Host
│   │   ├── app_controller.cpp            # Hiện thực luồng điều phối chính, kết nối tín hiệu Signal/Slot
│   │   ├── csv_exporter.cpp              # Hiện thực định dạng và xuất file CSV báo cáo
│   │   ├── cua_so_chinh.cpp              # Hiện thực tương tác giao diện, vẽ biểu đồ & hiển thị trạng thái
│   │   ├── dang_nhap.cpp                 # Hiện thực xác thực tài khoản & điều hướng người dùng
│   │   ├── database_service.cpp          # Hiện thực các truy vấn SQLite (CRUD Sensor, User, Alert)
│   │   ├── main.cpp                      # Điểm khởi chạy chương trình (Main Entrypoint)
│   │   ├── mqtt_service.cpp              # Hiện thực giao tiếp Mosquitto MQTT & phân tích JSON payload
│   │   ├── quan_ly_tai_khoan.cpp         # Hiện thực thêm/sửa/xóa và quản lý tài khoản người dùng
│   │   └── settings_service.cpp          # Hiện thực đọc/lưu cài đặt hệ thống vào cau_hinh.ini
│   └── ui/                               # Giao diện thiết kế trực quan Qt Designer (.ui)
│       ├── cua_so_chinh.ui               # Thiết kế màn hình giám sát chính (Dashboard, Charts, Logs)
│       ├── dang_nhap.ui                  # Thiết kế màn hình đăng nhập hệ thống
│       └── quan_ly_tai_khoan.ui          # Thiết kế cửa sổ quản lý tài khoản và phân quyền
├── scripts/                              # Kịch bản tự động hóa biên dịch & triển khai
│   ├── build_arm64.sh                    # Biên dịch chéo cho Raspberry Pi (Linux ARM64)
│   ├── build_host.sh                     # Biên dịch nhanh cho máy tính Host (Linux x86_64)
│   ├── deploy_pi.sh                      # Tự động hóa build, đóng gói và triển khai lên Pi qua SSH/SCP
│   ├── run_from_qtcreator.sh             # Cấu hình môi trường khi debug/chạy trực tiếp từ Qt Creator
│   └── run_host.sh                       # Script khởi chạy ứng dụng nhanh trên máy tính Host
├── test_esp/                             # Bộ kiểm thử đơn vị & kiểm thử tự động Firmware ESP32
│   ├── include/                          # Header files mock / API kiểm thử
│   ├── src/                              # Mã nguồn mock logic ESP32 phục vụ kiểm thử độc lập
│   ├── test/                             # Các ca kiểm thử Unit Test (BMP280, TSL2561, Relay, MQTT)
│   ├── unity/                            # Framework kiểm thử Unity C
│   ├── BAO_CAO_TEST_CASE_VA_KET_QUA.html # Báo cáo kết quả kiểm thử định dạng HTML
│   ├── BAO_CAO_TEST_CASE_VA_KET_QUA.md   # Báo cáo kết quả kiểm thử định dạng Markdown
│   ├── Makefile                          # Kịch bản build và chạy toàn bộ test case
│   ├── generate_report.py                # Script tự động trích xuất kết quả và sinh báo cáo
│   ├── run_tests.sh                      # Shell script thực thi chuỗi kiểm thử
│   └── README.md                         # Hướng dẫn chạy và giải thích bộ kiểm thử ESP32
├── .clang-format                         # Quy chuẩn định dạng mã nguồn C++ (Google Style)
├── .editorconfig                         # Thiết lập thống nhất định dạng file cho các IDE
├── .gitignore                            # Danh sách tệp tin và thư mục loại trừ khỏi Git
├── LICENSE                               # Giấy phép bản quyền mã nguồn mở MIT
└── README.md                             # Tài liệu giới thiệu & hướng dẫn sử dụng toàn diện
```

---

## 🔌 Sơ đồ Nối dây Phần cứng (Hardware Wiring)

| Thiết bị | Chân ESP32 | Mô tả chức năng |
| :--- | :--- | :--- |
| **BMP280 (I2C)** | `GPIO 21` (SDA) <br> `GPIO 22` (SCL) | Cảm biến đo Nhiệt độ & Áp suất khí quyển |
| **TSL2561 (I2C)** | `GPIO 21` (SDA) <br> `GPIO 22` (SCL) | Cảm biến đo Cường độ ánh sáng (Lux) |
| **Relay 1 (Quạt)** | `GPIO 26` | Rơ-le điều khiển Quạt làm mát (Active HIGH) |
| **Relay 2 (Đèn)** | `GPIO 27` | Rơ-le điều khiển Đèn chiếu sáng (Active HIGH) |
| **Nguồn cấp** | `5V / VIN` + `GND` | Nguồn cấp ổn định 5V-2A cho ESP32 và Relays |

> 📖 *Xem sơ đồ chi tiết và lưu ý địa chỉ I2C tại [docs/HARDWARE_PINOUT.md](docs/HARDWARE_PINOUT.md).*

---

## 📡 Đặc tả Giao thức MQTT (MQTT Topics Protocol)

| Topic | Hướng | Mô tả | Định dạng Payload |
| :--- | :---: | :--- | :--- |
| `tramthoitiet/data` | ESP32 ➔ Host | Dữ liệu cảm biến chu kỳ 2s | `{"temp":28.5,"press":1013.25,"lux":120.0,"fan":0,"light":1}` |
| `tramthoitiet/status` | ESP32 ➔ Host | Trạng thái kết nối / LWT | `{"status":"online","ip":"192.168.1.50"}` |
| `tramthoitiet/control` | Host ➔ ESP32 | Điều khiển rơ-le cưỡng bức | `{"device":"fan","state":"ON"}` |
| `tramthoitiet/threshold`| Host ➔ ESP32 | Cài đặt ngưỡng cảm biến | `{"temp_threshold":32.5,"light_threshold":50.0}` |
| `tramthoitiet/mode` | Host ➔ ESP32 | Chuyển chế độ Tự động/Thủ công | `{"mode":"AUTO"}` hoặc `{"mode":"MANUAL"}` |

> 📖 *Xem chi tiết ví dụ và quy tắc QoS/Retain tại [docs/MQTT_API.md](docs/MQTT_API.md).*

---

## 🚀 Hướng dẫn Cài đặt & Sử dụng (Getting Started)

### 1. Chuẩn bị môi trường (Prerequisites)

- **Trình biên dịch & Công cụ**: `gcc`/`g++` (hỗ trợ C++17), `cmake` (>= 3.18), `ninja-build`, `pkg-config`.
- **Thư viện Qt6**: `qt6-base-dev`, `qt6-tools-dev`.
- **MQTT Library & Broker**: `libmosquitto-dev`, `mosquitto`, `mosquitto-clients`.
- **Phần mềm nạp ESP32**: [PlatformIO Core](https://platformio.org/install/cli) hoặc VS Code Extension.

Trên hệ điều hành Ubuntu / Debian:
```bash
sudo apt update
sudo apt install -y build-essential cmake ninja-build pkg-config \
    qt6-base-dev qt6-tools-dev libmosquitto-dev mosquitto mosquitto-clients
```

---

### 2. Biên dịch & Nạp Firmware ESP32

1. Di chuyển vào thư mục `esp32`:
   ```bash
   cd esp32
   ```
2. Chỉnh sửa thông tin Wi-Fi và IP máy chủ MQTT trong tệp `include/cau_hinh.h`:
   ```cpp
   #define TEN_WIFI        "Your_SSID"
   #define MAT_KHAU_WIFI   "Your_Password"
   #define MQTT_BROKER     "192.168.1.100"  // IP máy chủ chạy Mosquitto
   ```
3. Biên dịch và nạp vào mạch:
   ```bash
   pio run -t upload
   pio device monitor -b 115200
   ```

---

### 3. Biên dịch & Chạy Ứng dụng Qt Host trên Desktop

1. Biên dịch ứng dụng bằng script tự động:
   ```bash
   ./scripts/build_host.sh
   ```
2. Khởi chạy ứng dụng:
   ```bash
   ./scripts/run_host.sh
   ```
3. Đăng nhập với tài khoản mặc định:
   - **Tài khoản**: `admin`
   - **Mật khẩu**: `123456`

---

### 4. Triển khai lên Raspberry Pi

1. Biên dịch chéo gói ARM64 (nếu sử dụng Toolchain cross-compiler):
   ```bash
   ./scripts/build_arm64.sh
   ```
2. Triển khai tự động qua mạng nội bộ SSH:
   ```bash
   ./scripts/deploy_pi.sh
   ```
