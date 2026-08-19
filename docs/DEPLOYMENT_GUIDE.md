# Hướng Dẫn Triển Khai Raspberry Pi (Raspberry Pi Deployment Guide)

Tài liệu hướng dẫn chi tiết cách thiết lập môi trường, biên dịch, triển khai và cấu hình tự động khởi động (Autostart/Systemd) ứng dụng **Trạm Quan Trắc Thời Tiết** trên Raspberry Pi 3 / 4 / 5.

---

## 1. Cài Đặt Các Gói Phụ Thuộc Trên Raspberry Pi

Mở Terminal trên Raspberry Pi và cài đặt các thư viện cần thiết:

```bash
sudo apt update
sudo apt install -y \
    qt6-base-dev \
    qt6-tools-dev \
    libmosquitto-dev \
    libsqlite3-dev \
    mosquitto \
    mosquitto-clients
```

---

## 2. Cấu Hình Mosquitto MQTT Broker Trên Pi

Cho phép kết nối từ bên ngoài (ESP32) bằng cách chỉnh sửa tệp cấu hình Mosquitto:

```bash
sudo nano /etc/mosquitto/conf.d/tram_thoi_tiet.conf
```

Thêm nội dung sau:
```ini
listener 1883
allow_anonymous true
```

Khởi động lại dịch vụ Mosquitto:
```bash
sudo systemctl enable mosquitto
sudo systemctl restart mosquitto
```

---

## 3. Triển Khai Ứng Dụng Lên Pi

### Cách 1: Triển khai từ máy Host qua SSH (Khuyến nghị)
Chỉnh sửa IP và tài khoản của Raspberry Pi trong tệp `scripts/deploy_pi.sh`, sau đó chạy:
```bash
./scripts/deploy_pi.sh
```

### Cách 2: Biên dịch trực tiếp trên Raspberry Pi
Clone repository trực tiếp trên Pi và chạy:
```bash
mkdir build && cd build
cmake -S ../qt -B . -GNinja -DCMAKE_BUILD_TYPE=Release
cmake --build . --parallel $(nproc)
```

---

## 4. Cấu Hình Tự Động Khởi Chạy Cùng Màn Hình (Autostart GUI Kiosk)

Tạo tệp `.desktop` tự động khởi chạy khi Raspberry Pi boot vào Desktop:

```bash
mkdir -p ~/.config/autostart
nano ~/.config/autostart/tram_thoi_tiet.desktop
```

Điền nội dung:
```ini
[Desktop Entry]
Type=Application
Name=Tram Thoi Tiet Qt
Exec=/home/pi/tram-thoi-tiet/scripts/run_pi.sh
Path=/home/pi/tram-thoi-tiet
Terminal=false
StartupNotify=false
```

---

## 5. Khắc Phục Sự Cố Thường Gặp (Troubleshooting)

| Hiện Tượng | Nguyên Nhân | Cách Xử Lý |
| :--- | :--- | :--- |
| **Không kết nối được MQTT Broker** | Mosquitto chưa bật hoặc bị firewall chặn | Kiểm tra bằng `sudo systemctl status mosquitto`, mở cổng 1883 trên ufw (`sudo ufw allow 1883`). |
| **Lỗi thư viện Qt `cannot open shared object`** | Thiếu gói Qt6 trên Pi | Chạy lại `sudo apt install -y qt6-base-dev`. |
| **ESP32 không gửi được dữ liệu** | Sai SSID/Password Wi-Fi hoặc sai IP Pi | Kiểm tra Serial Monitor của ESP32 qua cổng USB ở tốc độ 115200 bps. |
| **Database không lưu được** | Không có quyền ghi vào thư mục `data/` | Chạy `mkdir -p data && chmod 777 data`. |
