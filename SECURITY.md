# Chính Sách Bảo Mật (Security Policy)

## 🛡️ Phiên Bản Được Hỗ Trợ (Supported Versions)

Chúng tôi cung cấp các bản vá bảo mật cho các phiên bản phát hành sau:

| Phiên Bản | Được Hỗ Trợ |
| :--- | :---: |
| `1.0.x` | :white_check_mark: |
| `< 1.0.0` | :x: |

---

## 🔒 Khuyến Nghị Bảo Mật Trong Triển Khai IoT

Khi triển khai hệ thống **Trạm Quan Trắc Thời Tiết** trong môi trường thực tế, vui lòng lưu ý:

1. **Thông tin xác thực mạng & MQTT**:
   - **KHÔNG** đẩy mật khẩu Wi-Fi hoặc tài khoản MQTT thật lên GitHub công khai.
   - Sử dụng tệp mẫu cấu hình `esp32/include/cau_hinh.example.h` và cấu hình cục bộ an toàn.
   - Luôn thay đổi mật khẩu mặc định của Broker Mosquitto (`esp32 / 123`) và tài khoản Host Admin (`admin / admin123`).
2. **Mã hoá đường truyền**:
   - Trong môi trường sản xuất hoặc mạng công cộng ngoài Internet, khuyến nghị bật **MQTTS (TLS/SSL qua cổng 8883)** trên Mosquitto Broker và cấu hình chứng chỉ trên ESP32/Qt Host.
3. **Cơ sở dữ liệu SQLite**:
   - Thiết lập quyền truy cập tệp tin (File Permissions) chặt chẽ cho tệp `data/tram_thoi_tiet.sqlite` (`chmod 600`) trên Raspberry Pi / Host Linux.

---

## 🚨 Báo Cáo Lỗ Hổng Bảo Mật (Reporting a Vulnerability)

Nếu bạn phát hiện bất kỳ lỗ hổng bảo mật nào trong dự án:

1. **Vui lòng KHÔNG mở Issue công khai** trên GitHub.
2. Hãy gửi thông tin mô tả chi tiết kèm Proof of Concept (PoC) trực tiếp qua kênh liên hệ bảo mật của ban quản trị hoặc chức năng **Private Vulnerability Reporting** của GitHub.
3. Chúng tôi sẽ phản hồi trong vòng **48 giờ** và cung cấp bản vá trong thời gian sớm nhất có thể.
