# Sơ Đồ Nối Dây Phần Cứng (Hardware Pinout & Wiring)

Tài liệu này cung cấp sơ đồ kết nối chân (Pinout) và đặc tính kỹ thuật của phần cứng trong hệ thống **Trạm Quan Trắc Thời Tiết**.

---

## 1. Danh Sách Linh Kiện (Bill of Materials - BOM)

| Linh Kiện | Số Lượng | Chuẩn Giao Tiếp / Thông Số |
| :--- | :---: | :--- |
| **ESP32 NodeMCU (30 / 38 chân)** | 1 | Wi-Fi 2.4GHz + BLE, 3.3V Logic |
| **Cảm biến Bosch BMP280** | 1 | I2C (Mặc định `0x76` hoặc `0x77`), 3.3V |
| **Cảm biến Ánh sáng TSL2561** | 1 | I2C (Mặc định `0x39`, `0x29`, `0x49`), 3.3V |
| **Module 2 Relay Opto cách ly** | 1 | Kích mức cao (Active HIGH), 5V VCC |
| **Nguồn cấp 5V-2A Adapter** | 1 | Nguồn ổn định nuôi toàn bộ hệ thống |
| **Dây cắm testboard / Header** | 1 bộ | Dây Dupont Đực-Cái, Cái-Cái |

---

## 2. Bảng Nối Chân Chi Tiết (Pinout Table)

### 2.1 Cảm biến I2C (Bus I2C dùng chung SDA/SCL)

| Chân Module Cảm Biến | Chân ESP32 | Ghi Chú |
| :--- | :--- | :--- |
| **BMP280 - VCC** | `3V3` | Nguồn 3.3V (Không cắm 5V để tránh hỏng cảm biến) |
| **BMP280 - GND** | `GND` | Nối chung mass |
| **BMP280 - SCL** | `GPIO 22` | I2C Clock Bus |
| **BMP280 - SDA** | `GPIO 21` | I2C Data Bus |
| **TSL2561 - VCC** | `3V3` | Nguồn 3.3V |
| **TSL2561 - GND** | `GND` | Nối chung mass |
| **TSL2561 - SCL** | `GPIO 22` | I2C Clock Bus (Nối song song với BMP280) |
| **TSL2561 - SDA** | `GPIO 21` | I2C Data Bus (Nối song song với BMP280) |

### 2.2 Module Relay (Điều Khiển Cơ Cấu Chấp Hành)

| Chân Module Relay | Chân ESP32 | Ghi Chú |
| :--- | :--- | :--- |
| **Relay - VCC** | `5V / VIN` | Nguồn 5V cuộn hút rơ-le |
| **Relay - GND** | `GND` | Nối chung mass |
| **Relay - IN1 (Quạt)** | `GPIO 26` | Kích `HIGH` (3.3V) để đóng tiếp điểm rơ-le |
| **Relay - IN2 (Đèn)** | `GPIO 27` | Kích `HIGH` (3.3V) để đóng tiếp điểm rơ-le |

---

## 3. Địa Chỉ I2C Của Cảm Biến (I2C Addresses)

- **BMP280**:
  - Địa chỉ mặc định: `0x76` (khi chân `SDO` nối GND)
  - Địa chỉ phụ: `0x77` (khi chân `SDO` nối VCC)
- **TSL2561**:
  - Địa chỉ mặc định: `0x39` (khi chân `ADDR` để lơ lửng / FLOAT)
  - Địa chỉ phụ: `0x29` (ADDR -> GND) hoặc `0x49` (ADDR -> VCC)

---

## 4. Lưu Ý An Toàn & Thiết Kế Điện (Safety Notes)

1. **Nguồn cấp**:
   - Sử dụng nguồn 5V ngoài đủ dòng (tối thiểu 2A) nuôi Relay để tránh sụt áp gây Reset ESP32 khi 2 rơ-le cùng đóng một lúc.
2. **Cách ly cao áp**:
   - Phần điện xoay chiều (220VAC nếu tải đèn/quạt công suất lớn) phải được nối cách ly an toàn qua các cực `COM` và `NO` của rơ-le, có cầu chì bảo vệ chống ngắn mạch.
