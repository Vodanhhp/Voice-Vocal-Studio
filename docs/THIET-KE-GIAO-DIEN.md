# ĐỊNH HƯỚNG THIẾT KẾ GIAO DIỆN

## Phong cách
Bàn điều khiển phòng thu âm chuyên nghiệp, nền tối, bố cục rõ ràng. Ưu tiên thao tác nhanh và nhìn được trạng thái âm thanh.

## Các khu vực chính

### 1. Thiết bị âm thanh
- Thiết bị đầu vào (Micro/Input).
- Thiết bị đầu ra (Output).
- Driver/audio backend.
- Sample rate và buffer size.
- Trạng thái kết nối.

### 2. Mixer và mức tín hiệu
- Input Gain.
- Mic Volume.
- Output Volume.
- Send/Monitor level nếu phù hợp.
- Đồng hồ mức tín hiệu đầu vào và đầu ra.
- Cảnh báo tín hiệu quá nhỏ hoặc clipping.

### 3. EQ
- Các núm Bass, Mid, Treble.
- Có thể mở rộng thành EQ nhiều dải.
- Bật/tắt EQ và nút Reset.

### 4. Bộ xử lý động
- Compressor: Threshold, Ratio, Attack, Release, Makeup Gain.
- De-esser: Amount/Frequency.
- Limiter: Ceiling/Threshold.
- Mỗi module có công tắc bật/tắt và Bypass riêng.

### 5. Làm màu giọng
- Saturation nhẹ.
- Noise Reduction tùy chọn.
- Các điều khiển phải có giới hạn an toàn để tránh làm méo tiếng.

### 6. Preset và chế độ
- Karaoke.
- MC / Trò chuyện.
- Podcast.
- Livestream.
- Lưu, tải và so sánh A/B preset.

### 7. Điều khiển tổng
- Bật/tắt xử lý.
- Bypass toàn bộ.
- Hiển thị trạng thái và độ trễ.
- Không được nhầm trạng thái giao diện với trạng thái audio engine.

## Nguyên tắc thiết kế
- Dùng núm vặn cho thông số liên tục; dùng công tắc cho chức năng bật/tắt.
- Hiển thị giá trị số bên cạnh núm để chỉnh chính xác.
- Các khu vực có nhãn tiếng Việt dễ hiểu, kèm thuật ngữ tiếng Anh khi cần.
- Có tooltip hoặc giải thích ngắn cho thông số khó.
- Giao diện có thể nhiều điều khiển nhưng không dồn tất cả vào một màn hình chật; chia nhóm hợp lý.
- Không thêm điều khiển giả. Mỗi control phải được nối với tham số DSP hoặc trạng thái thiết bị thật trước khi công bố hoàn thành.
