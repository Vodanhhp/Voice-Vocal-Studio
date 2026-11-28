# TIẾN ĐỘ VOICE VOCAL STUDIO

**Cập nhật:** 27/11/2026  
**Trạng thái:** Môi trường JUCE/Visual Studio đã build và chạy thành công; đang hoàn thiện giao diện điều khiển trước khi triển khai audio engine.

## 1. Môi trường đã kiểm tra
- Windows trên máy Dell OptiPlex 5040.
- JUCE đã được đặt tại `E:\\JUCE`; thư viện module tại `E:\\JUCE\\modules`.
- Repository local: `E:\\Voice-Vocal-Studio`.
- Projucer mở được `VoiceVocalStudio.jucer`.
- Đã cấu hình đường dẫn module JUCE và lưu project.
- Visual Studio 2026: Rebuild Solution thành công.
- Ứng dụng chạy được bằng Ctrl+F5.

## 2. Giao diện hiện tại
Bản giao diện studio đang được dựng với:
- Nền tối, tiêu đề và trạng thái Audio Engine.
- Khu vực chọn thiết bị đầu vào/đầu ra (hiện mới là khung giao diện, chưa liệt kê thiết bị thật).
- Mixer: Input Gain, Mic Volume, Output.
- EQ: Bass, Mid, Treble.
- Compressor: Threshold, Ratio, Attack, Release.
- Làm sạch/màu giọng: De-esser, Saturation, Noise Reduction, Limiter.
- Preset lựa chọn: Karaoke, MC/Trò chuyện, Podcast, Livestream.
- Công tắc bật/tắt nhóm, nút xử lý tổng và Bypass.
- Khu vực hiển thị trạng thái mức tín hiệu.

**Giới hạn cần nhớ:** Đây là UI prototype. Các núm vặn có thể thay đổi giá trị trên giao diện nhưng chưa nối với DSP; công tắc/preset chưa thay đổi âm thanh; meter chưa lấy tín hiệu thật. Không được coi là xử lý giọng hát đã hoạt động.

## 3. Định hướng sản phẩm đã thống nhất
- Ứng dụng Windows xử lý giọng nói/giọng hát realtime, phục vụ karaoke online, MC, trò chuyện và livestream.
- Chất giọng hướng tới: nam ấm, dày, tự nhiên, rõ và truyền cảm.
- Không mặc định loại bỏ Echo/Reverb từ vang số hoặc sound card nếu hiệu ứng đó đang nghe hay.
- Tách đường người hát nghe trực tiếp và đường âm thanh xử lý gửi lên ứng dụng online.
- Chế độ mặc định ưu tiên hát đúng nhịp: người hát nghe qua Direct Monitoring của thiết bị ngoài; máy tính xử lý đường gửi online.
- Chế độ nghe tiếng đã xử lý qua PC chỉ bật khi độ trễ thực tế đủ thấp và có cảnh báo.
- Không hứa trước độ trễ tổng thể dưới 10 ms; phải đo trên thiết bị thực tế.
- Không tự giả định người dùng đã có ASIO, VB-CABLE hoặc thiết bị micro ảo.

## 4. Sơ đồ tín hiệu dự kiến
Micro → Vang số (nếu có) → Mixer/Sound Card → Voice Vocal Studio → Virtual Microphone → ứng dụng online.

Đây là sơ đồ định hướng; cần xác nhận thiết bị thực tế trước khi cấu hình chi tiết.

## 5. Việc tiếp theo
- [ ] Đồng bộ giao diện mới về máy và kiểm tra build.
- [ ] Điều chỉnh bố cục/hiển thị sau khi chạy trên máy người dùng.
- [ ] Kết nối JUCE AudioDeviceManager để liệt kê và chọn thiết bị âm thanh thật.
- [ ] Hiển thị input/output meter theo tín hiệu thật.
- [ ] Xây dựng pipeline audio realtime.
- [ ] Kết nối từng núm/công tắc với tham số DSP thực.
- [ ] Thêm EQ, compressor, de-esser, limiter và saturation; kiểm tra từng khối bằng tín hiệu thật.
- [ ] Đánh giá noise reduction, latency, clipping và độ ổn định.
- [ ] Bổ sung lưu/tải preset và A/B sau khi pipeline DSP ổn định.

## 6. Nguyên tắc làm việc
- Hướng dẫn từng bước nhỏ; người dùng thao tác và báo kết quả rồi mới đi tiếp.
- Không yêu cầu chụp ảnh nếu có thể đọc tên mục hoặc thông báo trên màn hình.
- Không ghi nhận chức năng là hoàn thành nếu mới chỉ có giao diện.
- Không tự ý thay đổi cấu hình GitHub hoặc project local ngoài phạm vi đã thống nhất.
- Cập nhật file TIENDO.md sau mỗi phiên làm việc quan trọng.

## 7. Cap nhat 28/11/2026 - Preset

- Da ket noi ba preset co san voi cac tham so DSP: Giong nam am ap, Giong nam day, Karaoke nhac nhe.
- Chon preset se nap lai thong so cho cac nhom xu ly va bat cac nhom xu ly.
- Da them luu cau hinh tuy chinh ra tep `.vvpreset` trong thu muc du lieu nguoi dung.
- Da them mo va nap lai tep preset tuy chinh.
- Tep preset luu gia tri cac nut dieu khien, trang thai bat/tat tung nhom va am luong tong.
- Chua xac nhan build va kiem thu tren may nguoi dung; can Rebuild Solution va thu lan luot ba preset, luu tep, mo tep.

## 8. Cap nhat 28/11/2026 - Mo rong lua chon chat giong

- Mo rong danh sach preset tu 3 len 9 lua chon.
- Giu nguyen ba preset ban dau: Giong nam am ap, Giong nam day, Karaoke nhac nhe.
- Them cac lua chon: Giong muot ma, Giong sang ro, Giong tram am, Giong san khau, Giong truyen cam, Karaoke tu nhien.
- Moi lua chon nap dong bo cac thong so EQ, Compressor, De-Esser, Voice Enhance, Noise Gate, Tone Control va Reverb; dong thoi cap nhat cac nut dieu khien tren giao dien.
- Cac gia tri moi la diem khoi dau de thu nghiem, can nghe va hieu chinh tren micro/soundcard thuc te.
- Chua xac nhan build va kiem thu tren may nguoi dung.

