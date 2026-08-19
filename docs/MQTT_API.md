# Đặc Tả Giao Thức MQTT (MQTT API Specification)

Tài liệu này quy định cấu trúc thông điệp, danh sách Topic và định dạng Payload giao tiếp giữa các thành phần trong hệ thống **Trạm Quan Trắc Thời Tiết**.

---

## 1. Thông Số Kết Nối Broker (Broker Connection Parameters)

- **Giao thức**: MQTT v3.1.1 (TCP)
- **Port mặc định**: `1883`
- **Mặc định Client ID**:
  - ESP32: `tram_thoi_tiet_esp32`
  - Qt Host: `tram_thoi_tiet_host_gui`
- **Xác thực**: Username / Password

---

## 2. Danh Sách Topics (Topic Reference)

### 2.1 `tramthoitiet/data` (ESP32 ➔ Broker ➔ Host)
- **Mô tả**: Dữ liệu cảm biến và trạng thái thiết bị định kỳ (chu kỳ mặc định: 2 giây).
- **QoS**: `0` hoặc `1`
- **Retain**: `false`
- **Payload Format (JSON)**:
```json
{
  "temp": 28.45,
  "press": 1012.30,
  "lux": 145.2,
  "fan": 1,
  "light": 0
}
```
**Giải thích trường**:
- `temp` (float): Nhiệt độ môi trường (°C) từ BMP280.
- `press` (float): Áp suất khí quyển (hPa) từ BMP280.
- `lux` (float): Cường độ ánh sáng (Lux) từ TSL2561.
- `fan` (int): Trạng thái rơ-le quạt (`1` = Bật, `0` = Tắt).
- `light` (int): Trạng thái rơ-le đèn (`1` = Bật, `0` = Tắt).

---

### 2.2 `tramthoitiet/status` (ESP32 ➔ Broker ➔ Host)
- **Mô tả**: Báo cáo tình trạng hoạt động và thông điệp di chúc (Last Will and Testament - LWT).
- **QoS**: `1`
- **Retain**: `true`
- **Payload khi Online**:
```json
{
  "status": "online",
  "client_id": "tram_thoi_tiet_esp32",
  "ip": "192.168.1.55",
  "rssi": -62,
  "uptime": 3600
}
```
- **Payload khi Offline (LWT)**:
```json
{
  "status": "offline",
  "client_id": "tram_thoi_tiet_esp32"
}
```

---

### 2.3 `tramthoitiet/control` (Host ➔ Broker ➔ ESP32)
- **Mô tả**: Điều khiển bật/tắt thiết bị ngoại vi tức thì (ở chế độ Manual hoặc Cưỡng bức).
- **QoS**: `1`
- **Retain**: `false`
- **Payload**:
```json
{
  "device": "fan",
  "state": "ON"
}
```
Hoặc:
```json
{
  "device": "light",
  "state": "OFF"
}
```
**Giá trị hợp lệ**:
- `device`: `"fan"` | `"light"` | `"all"`
- `state`: `"ON"` | `"OFF"` | `"TOGGLE"`

---

### 2.4 `tramthoitiet/threshold` (Host ➔ Broker ➔ ESP32)
- **Mô tả**: Cập nhật ngưỡng kích hoạt cảnh báo & tự động hoá trên mạch ESP32.
- **QoS**: `1`
- **Retain**: `true`
- **Payload**:
```json
{
  "temp_threshold": 32.5,
  "light_threshold": 55.0
}
```

---

### 2.5 `tramthoitiet/mode` (Host ➔ Broker ➔ ESP32)
- **Mô tả**: Chuyển đổi chế độ hoạt động của trạm quan trắc.
- **QoS**: `1`
- **Retain**: `true`
- **Payload**:
```json
{
  "mode": "TU_DONG"
}
```
Hoặc:
```json
{
  "mode": "THU_CONG"
}
```
