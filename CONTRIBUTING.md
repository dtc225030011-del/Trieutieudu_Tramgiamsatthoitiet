# Hướng Dẫn Đóng Góp (Contributing Guidelines)

Cảm ơn bạn đã quan tâm và muốn đóng góp cho dự án **Trạm Quan Trắc Thời Tiết**! Mọi sự đóng góp từ bạn—từ sửa lỗi chính tả, cải thiện tài liệu, đến phát triển tính năng mới—đều rất quý giá đối với chúng tôi.

---

## 📋 Quy Trình Đóng Góp (Workflow)

1. **Fork** repository này về tài khoản GitHub của bạn.
2. Tạo nhánh tính năng mới từ nhánh `main`:
   ```bash
   git checkout -b feature/ten-tinh-nang-moi
   # hoặc
   git checkout -b fix/ten-loi-can-sua
   ```
3. Thực hiện các chỉnh sửa, bổ sung mã nguồn và tài liệu.
4. Đảm bảo mã nguồn tuân thủ quy chuẩn định dạng và vượt qua kiểm thử:
   - Chạy `clang-format` cho C++:
     ```bash
     clang-format -i $(find qt/src qt/include esp32/src esp32/include -name "*.cpp" -o -name "*.h")
     ```
   - Kiểm tra build thử cả 2 phần:
     ```bash
     # Qt Host
     ./scripts/build_host.sh
     
     # ESP32
     cd esp32 && pio run
     ```
5. Commit thay đổi tuân theo quy chuẩn **Conventional Commits**:
   ```bash
   git commit -m "feat(gui): them bieu do thoi gian thuc"
   git commit -m "fix(esp32): sua loi timeout khi mat ket noi wifi"
   ```
6. Đẩy nhánh lên repository fork của bạn:
   ```bash
   git push origin feature/ten-tinh-nang-moi
   ```
7. Mở một **Pull Request (PR)** vào nhánh `main` của repository gốc và điền đầy đủ thông tin vào mẫu PR.

---

## 🏷️ Quy Chuẩn Commit Message (Conventional Commits)

Chúng tôi khuyến khích định dạng commit như sau:
```
<type>(<scope>): <short summary>

[optional body]

[optional footer(s)]
```

### Các tiền tố Type:
- `feat`: Tính năng mới cho người dùng.
- `fix`: Sửa lỗi trong code.
- `docs`: Thêm hoặc cập nhật tài liệu.
- `style`: Định dạng code, dấu cách, dấu chấm phẩy (không ảnh hưởng logic chạy).
- `refactor`: Tái cấu trúc mã nguồn mà không sửa lỗi hay thêm tính năng.
- `perf`: Cải thiện hiệu năng xử lý.
- `test`: Thêm hoặc sửa đổi bộ test.
- `ci`: Thay đổi cấu hình CI/CD (GitHub Actions).
- `chore`: Cập nhật build script, dependencies, hoặc công cụ phụ trợ.

---

## 💻 Chuẩn Mã Nguồn C++ (Code Style Guidelines)

- **Chuẩn C++**: C++17 cho ứng dụng Qt và C++11/14 cho ESP32 Arduino Core.
- **Quy tắc đặt tên**:
  - Class: `PascalCase` (ví dụ: `DatabaseService`, `MqttService`, `AppController`).
  - Hàm / Phương thức: `camelCase` hoặc `snake_case` thống nhất theo module.
  - Biến thành viên: `m_variableName` hoặc `snake_case`.
  - Hằng số / Macro: `ALL_CAPS_WITH_UNDERSCORES`.
- **Bộ nhớ & Con trỏ**: Sử dụng Smart Pointers (`std::unique_ptr`, `std::shared_ptr`) hoặc cơ chế cha con `QObject` của Qt để tránh rò rỉ bộ nhớ (memory leaks).
- **An toàn luồng**: Truy cập dữ liệu đa luồng (ví dụ nhận dữ liệu MQTT từ luồng nền) phải được điều phối qua Qt Signals & Slots hoặc Mutex an toàn.

---

## 🐛 Báo Cáo Lỗi (Reporting Bugs)

Khi phát hiện lỗi, hãy mở một Issue trên GitHub với các thông tin:
- Tiêu đề ngắn gọn, mô tả chính xác hiện tượng.
- Môi trường chạy (Hệ điều hành, phiên bản Qt, phiên bản ESP32 Arduino Core).
- Các bước tái hiện lỗi (Step-by-step reproduction steps).
- Log lỗi hoặc ảnh chụp màn hình minh hoạ.

---

## 💡 Đề Xuất Tính Năng Mới (Feature Requests)

Hãy tạo một Issue chọn mẫu `Feature Request` và mô tả:
- Vấn đề bạn muốn giải quyết hoặc tính năng muốn bổ sung.
- Giải pháp đề xuất và luồng hoạt động kỳ vọng.
- Bất kỳ tài liệu hoặc thiết bị phần cứng liên quan.
