# Kiến Trúc Hệ Thống (System Architecture)

Tài liệu này mô tả chi tiết thiết kế kiến trúc phần mềm, tổ chức module, luồng dữ liệu và cơ chế lưu trữ của dự án **Trạm Quan Trắc Thời Tiết**.

---

## 1. Tổng Quan Kiến Trúc (Architecture Overview)

Hệ thống được thiết kế theo mô hình **Event-driven IoT Architecture** gồm 3 tầng chính:

```mermaid
graph TD
    subgraph Edge Layer (ESP32)
        BMP[Cảm biến BMP280\nI2C: 0x76/0x77] -->|Đo Nhiệt độ & Áp suất| MCU[ESP32 Microcontroller]
        TSL[Cảm biến TSL2561\nI2C: 0x39] -->|Đo Ánh sáng Lux| MCU
        MCU -->|GPIO 26| R1[Relay 1: Quạt]
        MCU -->|GPIO 27| R2[Relay 2: Đèn]
    end

    subgraph Transport Layer
        MCU -->|MQTT Publish: data, status| Broker[Mosquitto MQTT Broker\nPort: 1883]
        Broker -->|MQTT Subscribe: control, threshold, mode| MCU
    end

    subgraph Application Layer (Qt6 Host)
        Broker -->|MQTT Message Event| MqttSvc[MqttService\nlibmosquitto]
        MqttSvc -->|Signals| AppCtrl[AppController]
        AppCtrl -->|Update View| MainWindow[Cửa Sổ Chính / GUI]
        AppCtrl -->|Log History| DbSvc[DatabaseService\nSQLite3 Engine]
        AppCtrl -->|Export Data| CsvExp[CsvExporter]
        AppCtrl -->|Manage Auth| AuthSvc[QuanLyTaiKhoan\nRBAC]
        AppCtrl -->|Publish Commands| MqttSvc
    end
```

---

## 2. Các Module Phần Mềm (Software Modules)

### 2.1 Node Biên (ESP32 Firmware)
- **`bmp280.cpp / bmp280.h`**: Giao tiếp cảm biến Bosch BMP280 qua I2C để đọc nhiệt độ (°C), áp suất (hPa) và tính toán độ cao tương đối.
- **`tsl2561.cpp / tsl2561.h`**: Giao tiếp cảm biến quang phổ TSL2561 qua I2C để đo cường độ sáng chính xác (Lux).
- **`relay.cpp / relay.h`**: Quản lý chân xuất GPIO điều khiển rơ-le kích mức cao (Active HIGH).
- **`wifi_mqtt.cpp / wifi_mqtt.h`**: Quản lý kết nối Wi-Fi (WPA2) và client MQTT (`PubSubClient`) với cơ chế tái kết nối không chặn (non-blocking).
- **`xu_ly_lenh.cpp / xu_ly_lenh.h`**: Phân tích cú pháp JSON payload từ các topic điều khiển (`tramthoitiet/control`, `tramthoitiet/threshold`, `tramthoitiet/mode`) và thực thi tương ứng.

### 2.2 Máy Chủ Giám Sát (Qt6 Host GUI Application)
- **`AppController` (`app_controller.h / .cpp`)**: Đóng vai trò Mediator / Controller trung tâm, điều phối sự kiện giữa UI, MQTT, Database và cấu hình.
- **`MqttService` (`mqtt_service.h / .cpp`)**: Đóng gói thư viện `libmosquitto`, chạy luồng xử lý nhận thông điệp (network loop) và phát tín hiệu `Qt Signals` khi có dữ liệu mới.
- **`DatabaseService` (`database_service.h / .cpp`)**: Quản lý kết nối `QSqlDatabase` (SQLite), khởi tạo bảng dữ liệu, ghi log lịch sử cảm biến và cảnh báo.
- **`SettingsService` (`settings_service.h / .cpp`)**: Đọc và lưu trữ cấu hình hệ thống ra tệp `cau_hinh.ini` (QSettings).
- **`CsvExporter` (`csv_exporter.h / .cpp`)**: Truy vấn dữ liệu lịch sử từ SQLite và kết xuất ra file định dạng CSV chuẩn UTF-8.
- **`QuanLyTaiKhoan` & `DangNhap`**: Xử lý đăng nhập, mã hoá thông tin xác thực và phân quyền người dùng theo vai trò.
- **`CuaSoChinh` (`cua_so_chinh.ui / .h / .cpp`)**: Giao diện hiển thị trực quan thông số, trạng thái rơ-le, đồ thị thời gian thực và bảng điều khiển.

---

## 3. Luồng Dữ Liệu (Data Flow)

### 3.1 Luồng Thu Thập & Cập Nhật Dữ Liệu
1. ESP32 định kỳ **2000ms** đọc dữ liệu từ cảm biến BMP280 và TSL2561.
2. ESP32 đóng gói dữ liệu thành chuỗi JSON và Publish lên topic `tramthoitiet/data`.
3. `Mosquitto Broker` chuyển tiếp gói tin đến `Qt Host Application`.
4. `MqttService` nhận dữ liệu, giải mã và emit tín hiệu `sensorDataReceived(SensorData)`.
5. `AppController` nhận tín hiệu:
   - Cập nhật số liệu tức thời lên `CuaSoChinh`.
   - Ghi bản ghi vào cơ sở dữ liệu `SQLite3`.
   - So khớp với ngưỡng cảnh báo cài đặt để hiển thị cảnh báo nếu vượt ngưỡng.

### 3.2 Luồng Điều Khiển Thiết Bị
1. Người dùng thao tác trên GUI (hoặc hệ thống tự động kích hoạt khi ở chế độ `TU_DONG`).
2. `AppController` gọi `MqttService::publishControl(...)`.
3. Thông điệp được gửi lên Broker qua topic `tramthoitiet/control`.
4. ESP32 nhận lệnh, cập nhật trạng thái GPIO rơ-le và phản hồi trạng thái mới nhất lên topic `tramthoitiet/data`.

---

## 4. Thiết Kế Cơ Sở Dữ Liệu (Database Schema)

Cơ sở dữ liệu SQLite lưu tại `data/tram_thoi_tiet.sqlite`:

```sql
-- Bảng lưu lịch sử dữ liệu cảm biến
CREATE TABLE IF NOT EXISTS sensor_history (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    timestamp DATETIME DEFAULT CURRENT_TIMESTAMP,
    temperature REAL,
    pressure REAL,
    light REAL,
    fan_state INTEGER,
    light_state INTEGER
);

-- Bảng lưu lịch sử cảnh báo
CREATE TABLE IF NOT EXISTS alert_history (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    timestamp DATETIME DEFAULT CURRENT_TIMESTAMP,
    alert_type TEXT,
    description TEXT,
    value REAL,
    threshold REAL
);

-- Bảng quản lý người dùng
CREATE TABLE IF NOT EXISTS users (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    username TEXT UNIQUE NOT NULL,
    password_hash TEXT NOT NULL,
    role TEXT NOT NULL,
    created_at DATETIME DEFAULT CURRENT_TIMESTAMP
);
```
