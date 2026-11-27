# Voice Vocal Studio

Phần mềm xử lý giọng nói và giọng hát theo thời gian thực trên Windows.

## Mục tiêu
- Tạo giọng nam ấm, dày, tự nhiên và rõ ràng.
- Phục vụ karaoke online, MC, trò chuyện và livestream.
- Có các bộ xử lý âm thanh thực sự hoạt động; không chỉ là giao diện minh họa.
- Ưu tiên độ ổn định, dễ sử dụng và khả năng cấu hình thiết bị âm thanh.

## Công nghệ định hướng
- C++ và JUCE.
- Windows 10/11.
- WASAPI; hỗ trợ ASIO khi thiết bị có driver phù hợp.
- Tần số lấy mẫu và buffer sẽ được kiểm tra trên thiết bị thực tế.

## Tình trạng
Xem [TIENDO.md](TIENDO.md) để biết trạng thái mới nhất và việc cần làm tiếp theo.

## Thiết kế
Xem [docs/THIET-KE-GIAO-DIEN.md](docs/THIET-KE-GIAO-DIEN.md).

## Lưu ý
Các nút điều khiển chỉ được coi là hoàn thành khi đã kết nối với xử lý âm thanh thật và kiểm tra được bằng tín hiệu đầu vào/đầu ra.
