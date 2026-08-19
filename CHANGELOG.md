# Nhật Ký Thay Đổi (Changelog)

Tất cả các thay đổi đáng chú ý của dự án **Trạm Quan Trắc Thời Tiết** sẽ được ghi lại trong tài liệu này.

Định dạng nhật ký tuân thủ theo [Keep a Changelog](https://keepachangelog.com/vi/1.0.0/) và tuân theo chuẩn [Semantic Versioning](https://semver.org/lang/vi/).

---

## [1.0.0] - 2026-08-19

### 🎉 Tính Năng Mới (Added)
- **Node Cảm Biến ESP32 (Firmware)**:
  - Tích hợp driver I2C cho cảm biến nhiệt độ & áp suất khí quyển `BMP280`.
  - Tích hợp driver I2C cho cảm biến đo cường độ sáng quang phổ `TSL2561`.
  - Hỗ trợ đóng ngắt 2 rơ-le độc lập: Quạt tản nhiệt (`GPIO 26`) và Đèn chiếu sáng (`GPIO 27`).
  - Giao thức truyền thông MQTT theo định dạng JSON với cơ chế tự động kết nối lại (Auto-reconnect).
  - Điều khiển 2 chế độ: Tự động (theo ngưỡng nhiệt độ và ánh sáng) hoặc Thủ công qua MQTT topic.
- **Ứng Dụng Giám Sát Qt6 Host GUI**:
  - Giao diện người dùng đồ họa hiện đại với Qt6 Widgets.
  - Hệ thống xác thực đăng nhập và phân quyền người dùng (Admin, Manager, Operator).
  - Tích hợp `libmosquitto` bất đồng bộ để nhận dữ liệu và gửi lệnh điều khiển.
  - Lưu trữ dữ liệu lịch sử đo đạc vào cơ sở dữ liệu `SQLite3`.
  - Chức năng xuất dữ liệu báo cáo ra tệp `CSV` theo khoảng thời gian tùy chọn.
  - Hỗ trợ đọc/ghi cấu hình linh hoạt qua tệp `cau_hinh.ini`.
- **Tự Động Hoá & Triển Khai (DevOps & Scripts)**:
  - Script biên dịch ứng dụng cục bộ x86_64 (`scripts/build_host.sh`).
  - Script biên dịch chéo cho Raspberry Pi ARM64 (`scripts/build_arm64.sh`).
  - Script triển khai tự động qua SSH lên Raspberry Pi (`scripts/deploy_pi.sh`).
  - GitHub Actions CI/CD workflows kiểm thử tự động cho cả Qt và PlatformIO ESP32.
- **Tài Liệu Kỹ Thuật Toàn Diện**:
  - `README.md`, `ARCHITECTURE.md`, `MQTT_API.md`, `HARDWARE_PINOUT.md`, `DEPLOYMENT_GUIDE.md`.
