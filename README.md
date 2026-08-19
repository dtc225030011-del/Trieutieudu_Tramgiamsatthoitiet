# 🌦️ Trạm Giám Sát Thời Tiết

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
├── .github/                      # GitHub Actions CI/CD & Issue/PR Templates
│   ├── ISSUE_TEMPLATE/
│   │   ├── bug_report.md
│   │   └── feature_request.md
│   ├── PULL_REQUEST_TEMPLATE.md
│   └── workflows/
│       ├── code-quality.yml
│       ├── esp32-ci.yml
│       └── qt-ci.yml
├── docs/                         # Tài liệu kỹ thuật chi tiết
│   ├── ARCHITECTURE.md           # Kiến trúc phần mềm & luồng dữ liệu
│   ├── DEPLOYMENT_GUIDE.md       # Hướng dẫn triển khai Raspberry Pi / Linux
│   ├── HARDWARE_PINOUT.md        # Sơ đồ nối dây & thông số phần cứng
│   └── MQTT_API.md               # Đặc tả giao thức MQTT & cấu trúc Payload
├── esp32/                        # PlatformIO Firmware cho ESP32
│   ├── include/                  # Header files cấu hình & driver
│   │   ├── bmp280.h
│   │   ├── cau_hinh.h            # File cấu hình Wi-Fi, MQTT, chân GPIO
│   │   ├── cau_hinh.example.h    # File mẫu cấu hình
│   │   ├── relay.h
│   │   ├── tsl2561.h
│   │   ├── wifi_mqtt.h
│   │   └── xu_ly_lenh.h
│   ├── src/                      # Mã nguồn C++ Arduino/ESP32
│   │   ├── bmp280.cpp
│   │   ├── main.cpp
│   │   ├── relay.cpp
│   │   ├── tsl2561.cpp
│   │   ├── wifi_mqtt.cpp
│   │   └── xu_ly_lenh.cpp
│   └── platformio.ini            # Cấu hình PlatformIO & thư viện phụ thuộc
├── package/                      # Gói ứng dụng đóng gói triển khai cho Pi
│   └── tram-thoi-tiet/
│       ├── config/cau_hinh.ini   # File cấu hình INI của Host App
│       ├── data/                 # Thư mục lưu database SQLite
│       └── scripts/run_pi.sh     # Script khởi chạy ứng dụng trên Pi
├── qt/                           # Ứng dụng Qt6 C++ Host GUI
│   ├── CMakeLists.txt            # CMake build script
│   ├── include/                  # Header files của Qt Host
│   │   ├── app_controller.h
│   │   ├── cua_so_chinh.h
│   │   ├── dang_nhap.h
│   │   ├── database_service.h
│   │   ├── mqtt_service.h
│   │   └── ...
│   ├── resources/                # Icons & assets QRC
│   ├── src/                      # Mã nguồn triển khai Qt Host
│   └── ui/                       # Giao diện Qt Designer (.ui)
├── scripts/                      # Các scripts tự động hoá build & deploy
│   ├── build_arm64.sh            # Biên dịch chéo cho Raspberry Pi (ARM64)
│   ├── build_host.sh             # Biên dịch cho máy tính Host x86_64
│   ├── deploy_pi.sh              # Script đẩy gói và cài đặt tự động lên Pi qua SSH
│   ├── run_from_qtcreator.sh     # Chạy nhanh từ Qt Creator
│   └── run_host.sh               # Chạy ứng dụng trên môi trường Host
├── .clang-format                 # Quy chuẩn định dạng mã nguồn C++
├── .editorconfig                 # Quy chuẩn trình soạn thảo mã nguồn
├── .gitignore                    # Các tệp tin loại trừ khỏi Git
├── CHANGELOG.md                  # Nhật ký thay đổi các phiên bản
├── CODE_OF_CONDUCT.md            # Quy tắc ứng xử cộng đồng
├── CONTRIBUTING.md               # Hướng dẫn đóng góp mã nguồn
├── LICENSE                       # Giấy phép phần mềm MIT
└── README.md                     # Tài liệu chính của dự án
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
   - **Mật khẩu**: `admin123`

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
