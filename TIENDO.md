# TIẾN ĐỘ VOICE VOCAL STUDIO

**Cập nhật:** 27/11/2026  
**Trạng thái:** Đã tạo và chạy thử dự án JUCE GUI cơ bản; đang chuẩn bị thiết kế lại giao diện trước khi triển khai xử lý âm thanh.

## 1. Môi trường đã cài và kiểm tra
- Windows trên máy Dell OptiPlex 5040.
- Đã tải và giải nén JUCE.
- Đã mở được Projucer.
- Đã tạo dự án dạng GUI Application.
- Exporter đã chọn: Visual Studio 2026.
- Đã mở project trong Visual Studio.
- Build thành công.
- Chạy bằng Ctrl+F5 và cửa sổ Hello World xuất hiện.
- Đã thay nội dung MainComponent.h và MainComponent.cpp bằng giao diện thử nghiệm đầu tiên.
- Build và chạy lại thành công.

## 2. Cấu trúc code hiện tại trên máy
Các file đã chỉnh sửa trong Visual Studio:
- MainComponent.h
- MainComponent.cpp

Giao diện hiện tại mới là bản thử nghiệm: tiêu đề, chọn thiết bị (chưa kết nối thiết bị thật), trạng thái tín hiệu, chọn chế độ Karaoke/MC, nút bật xử lý và Bypass. Các nút hiện chỉ thay đổi trạng thái giao diện, chưa xử lý âm thanh.

**Lưu ý:** Chưa đưa bản code giao diện thử nghiệm này lên GitHub vì giao diện đang được thống nhất lại. Không được hiểu các nút hiện tại là chức năng DSP đã hoạt động.

## 3. Định hướng sản phẩm đã thống nhất
- Ứng dụng Windows xử lý giọng nói/giọng hát realtime.
- Mục tiêu sử dụng: karaoke online, đặc biệt có thể gửi tiếng đã xử lý vào ứng dụng trò chuyện/online.
- Chất giọng hướng tới: nam ấm, dày, tự nhiên, rõ và truyền cảm.
- Không mặc định loại bỏ Echo/Reverb từ vang số hoặc sound card nếu hiệu ứng đó đang nghe hay.
- Cần tách đường người hát nghe trực tiếp và đường âm thanh xử lý gửi lên máy tính/ứng dụng online.
- Chế độ mặc định ưu tiên hát đúng nhịp: người hát nghe qua Direct Monitoring của thiết bị ngoài; máy tính xử lý đường gửi online.
- Chế độ nghe tiếng đã xử lý qua PC chỉ bật khi độ trễ thực tế đủ thấp và có cảnh báo.
- Không hứa trước độ trễ tổng thể dưới 10 ms; phải đo trên thiết bị thực tế.
- Không tự giả định người dùng đã có ASIO, VB-CABLE hoặc thiết bị micro ảo.

## 4. Định hướng giao diện
Người dùng muốn giao diện:
- Chuyên nghiệp, giống bàn điều khiển phòng thu.
- Nền tối.
- Có nhiều nút, công tắc bật/tắt, núm vặn âm lượng và thông số.
- Có đồng hồ mức tín hiệu.
- Có khu vực riêng cho thiết bị, mixer, EQ và các bộ xử lý.
- Tiếng Việt có dấu; có thể kèm tên kỹ thuật tiếng Anh.
- Không làm núm vặn/nút giả để trang trí: khi hoàn thiện phải điều khiển được xử lý âm thanh thật.

Xem thêm: [Thiết kế giao diện](docs/THIET-KE-GIAO-DIEN.md).

## 5. Các nhóm chức năng dự kiến
1. Chọn thiết bị đầu vào/đầu ra và cấu hình audio.
2. Đồng hồ tín hiệu đầu vào/đầu ra.
3. Gain và mixer.
4. EQ: Bass, Mid, Treble và các dải tần chi tiết.
5. Compressor.
6. De-esser.
7. Limiter.
8. Saturation nhẹ để làm dày giọng.
9. Noise reduction ở mức vừa phải, có thể bật/tắt.
10. Preset Karaoke, MC, Podcast, Livestream.
11. Bypass, A/B và lưu preset.
12. Auto Voice Optimizer: phân tích giọng mẫu khoảng 20–30 giây để đề xuất thông số; không quảng cáo là biến người dùng thành ca sĩ.

## 6. Sơ đồ tín hiệu dự kiến
Micro → Vang số (nếu có) → Mixer/Sound Card → Voice Vocal Studio → Virtual Microphone → ứng dụng online.

Đây là sơ đồ định hướng; cần xác nhận thiết bị thực tế trước khi cấu hình chi tiết.

## 7. Việc tiếp theo
- [ ] Chốt bố cục giao diện bàn điều khiển nền tối.
- [ ] Thiết kế lại MainComponent với tiếng Việt có dấu.
- [ ] Kiểm tra encoding UTF-8 và cấu hình compiler phù hợp trong Visual Studio.
- [ ] Kết nối JUCE AudioDeviceManager để liệt kê và chọn thiết bị âm thanh thật.
- [ ] Hiển thị input/output meter theo tín hiệu thật.
- [ ] Xây dựng pipeline audio realtime.
- [ ] Thêm từng DSP module và kiểm tra bằng micro thực tế.
- [ ] Kiểm tra latency, tiếng rè, clipping và độ ổn định.
- [ ] Sau khi xác nhận cấu trúc project JUCE tại máy, bổ sung đầy đủ file project cần thiết lên GitHub.

## 8. Nguyên tắc làm việc
- Hướng dẫn từng bước nhỏ; người dùng thao tác và báo kết quả rồi mới đi tiếp.
- Không yêu cầu chụp ảnh nếu có thể đọc tên mục hoặc thông báo trên màn hình.
- Không ghi nhận chức năng là hoàn thành nếu mới chỉ có giao diện.
- Không tự ý thay đổi cấu hình GitHub hoặc project local ngoài phạm vi đã thống nhất.
- Cập nhật file TIENDO.md sau mỗi phiên làm việc quan trọng.
