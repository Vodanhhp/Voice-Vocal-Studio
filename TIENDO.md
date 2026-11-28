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


## 9. Project checkpoint 28/11/2026 - Advanced vocal modules

### Confirmed product/UI direction
- The primary product goal is to improve singing voice quality and provide convincing, controllable voice-character changes.
- Advanced effects will be implemented as separate modules.
- Each module will have its own dedicated window or panel, opened only when the user clicks that module's button.
- Keep the main studio screen compact and uncluttered; do not place every advanced control on the main screen.
- Modules should be independently enableable/disableable and should cooperate safely in the realtime audio chain.
- Provide simple coordinated vocal modes/presets for ordinary users, while retaining deeper controls inside each module for advanced users.
- Preserve the existing working audio engine and DSP modules; add new processing as isolated modules rather than rewriting working code.
- Keep source/UI text ASCII-only for now to avoid encoding problems.

### Current implementation status
- Realtime JUCE audio device handling and the existing main studio UI are implemented.
- Input/output gain, monitor, processing and bypass routing are implemented.
- Five-band parametric EQ and its calculated response curve are implemented.
- Compressor, De-Esser, Voice Enhance, Noise Gate, Tone Control and Reverb DSP are implemented.
- Three original built-in presets and custom preset save/load are implemented.
- The built-in preset list has recently been expanded to nine voice choices. This latest expansion has NOT yet been confirmed by the user's local build and listening tests.
- The user has tested the earlier DSP blocks individually and confirmed audible operation for EQ, Compressor, Voice Enhance, Noise Gate, Tone Control and Reverb. De-Esser was accepted provisionally and may need further listening tests.
- Do not claim the nine-preset expansion is tested until the user reports a successful build and test.

### Planned advanced modules and implementation order
1. Pitch Correction / Auto-Tune:
   - Real-time pitch detection and correction with adjustable correction strength.
   - Provide key/scale selection; do not promise reliable musical correction without a suitable key/scale or other musical context.
   - Compare proven DSP implementations before choosing whether to build internally or add a dependency.
2. Vocal Leveling:
   - Improve vocal loudness consistency while preserving natural dynamics.
   - Review the current compressor before adding a separate rider/leveling stage.
3. Harmonic Saturation / Exciter:
   - Add controlled harmonics for warmth, density and presence without harsh clipping.
4. Pitch and Formant Shifter:
   - Support stylized lower/deeper and higher/feminine vocal characters.
   - Pitch and formant must be treated as separate controls; pitch alone does not create a convincing timbre change.
   - Evaluate sound quality and latency carefully; do not promise realistic identity conversion.
5. Vocal Doubler and Harmonizer:
   - Add vocal thickening/widening and harmony voices after the core pitch/formant path is stable.
6. Character Voice Effects:
   - Robot, Radio/Telephone and Alien/Character effects.
   - These are creative effects and can be scheduled after the core singing-quality modules.

### Engineering rules for new modules
- First inspect the current JUCE version, project configuration and existing audio callback before selecting libraries or changing dependencies.
- Keep realtime audio processing safe: avoid memory allocation, file I/O, blocking locks and expensive setup in the audio callback.
- Prepare buffers, filters and processing state outside the callback where possible.
- Test each module independently, then test combinations for CPU load, latency, clicks, dropouts and clipping.
- Prefer proven DSP libraries for difficult pitch/formant processing, but inspect license, compatibility, realtime suitability and maintenance before adding any dependency.
- Add one module at a time. Build and listen-test on the user's actual microphone and audio interface before moving to the next module.
- Do not redesign the main UI or alter existing working DSP without a specific reason and approval.

### Immediate next steps
- [ ] User pulls the latest main branch and builds/tests all nine built-in presets.
- [ ] Fetch and inspect the latest MainComponent.h, MainComponent.cpp and VoiceVocalStudio.jucer before any code changes.
- [ ] Confirm the JUCE version and current audio callback structure.
- [ ] Design a reusable module-window pattern and agree how module buttons open/close their dedicated controls.
- [ ] Evaluate pitch-correction DSP options and their latency/licensing before implementation.
- [ ] Implement and test the first advanced module as a separate, isolated unit.
