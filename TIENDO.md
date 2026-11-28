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
- Nguoi dung da Rebuild va kiem tra ca 9 preset; cac nut dieu khien cap nhat dong bo dung nhu mong doi.


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
- The built-in preset list has nine voice choices. The user has rebuilt and tested all nine; selecting each preset updates the corresponding controls as expected.
- The user has tested the earlier DSP blocks individually and confirmed audible operation for EQ, Compressor, Voice Enhance, Noise Gate, Tone Control and Reverb. De-Esser was accepted provisionally and may need further listening tests.
- Nine-preset expansion is confirmed tested by the user on 28/11/2026.

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


### Implementation update 28/11/2026 - First module window scaffold
- Added a dedicated PITCH CORRECTION button to the main studio header.
- Clicking the button opens a separate, resizable Pitch Correction module window.
- Added the planned key, scale, correction amount and correction speed controls to establish the module layout.
- These controls are deliberately disabled and clearly marked as not connected to DSP yet; this is UI scaffolding only, not pitch correction processing.
- The module window is implemented as a separate content component so the same window pattern can be reused for future vocal modules.
- User build/test is pending.
- Next: user verifies the new window opens and closes correctly; then evaluate pitch-detection/correction DSP options and implement the audio processing separately.

### Implementation update 28/11/2026 - Pitch tracking stage
- Connected the Pitch Correction module controls to shared realtime parameters: song key, scale, correction amount and correction speed.
- Added a fixed-size, allocation-free monophonic pitch-analysis ring buffer and periodic normalized autocorrelation tracking in the audio callback.
- Added detected-note, nearest in-scale note and cents-offset readouts in the module window.
- Major, Natural Minor and Chromatic scale target selection are reflected in the analysis.
- Important: this is the pitch-detection and target-analysis stage only. It does not yet shift the audible signal or correct sung notes. Correction Amount and Speed are stored and ready for the next DSP stage; they are not yet audible controls.
- User must rebuild and test note tracking with a clean, single vocal/mic input. Polyphonic music, strong reverb, noise and very breathy/unvoiced sounds can confuse a monophonic pitch detector.
- Next: improve pitch tracking confidence/accuracy, then add a dedicated low-latency pitch-shifting engine before enabling audible correction.

### Implementation update 28/11/2026 - Experimental audible pitch correction

- Added a fixed-size dual-delay pitch shifter to the realtime audio path. The implementation uses two crossfaded delay taps with a moving read position to shift pitch without changing the playback rate.
- The shifter uses a preallocated 8192-sample buffer and performs no allocation or locking in the audio callback.
- Added an independent Pitch Correction ON/OFF toggle inside the Pitch Correction module. The main BAT XU LY control and Bypass still act as the overall processing master.
- Correction Amount now scales the measured cents offset toward the selected scale note.
- Correction Speed controls the smoothing time of the pitch-ratio changes.
- The pitch shift is limited to approximately plus/minus one semitone to reduce extreme artifacts.
- Delay taps move between approximately 3 ms and 18 ms. These are internal read delays, not a guaranteed end-to-end system latency measurement.
- The shifter continues to keep its delay buffer warm while disabled, then fades the processed signal in/out to reduce abrupt switching.
- This is an experimental dual-delay pitch shifter, not a formant-preserving PSOLA or phase-vocoder engine. It can produce chorus/granular artifacts, alter vocal formants, and sound unnatural on consonants, vibrato, noisy or polyphonic input.
- The source has been checked for ASCII-only text and the expected code paths have been verified by fetching the updated files from main.
- Build and listening tests have NOT yet been performed on the user's computer. Do not mark this module production-ready until the user rebuilds and tests it with a dry, single-voice microphone signal.
- Next test: compare OFF and ON at low Correction Amount first; then test stable sustained notes, note transitions, vibrato and the selected key/scale. Listen for pitch stability, metallic/chorus artifacts, clicks and latency.


### Cap nhat 28/11/2026 - Hang nut vocal va Vocal Leveling
- Giu nguyen 8 nhom xu ly chinh tren giao dien; khong di chuyen cac panel Dau vao/Dau ra, EQ, Compressor, De-Esser, Voice Enhance, Noise Gate, Tone Control va Reverb.
- Dua Pitch Correction xuong hang nut rieng o phia duoi, cung cac nut Vocal Leveling, Vocal Limiter, Warmth & Body, Vocal Clarity, Breath Control va Vocal Exciter.
- Moi nut mo mot cua so rieng. Cac cua so cua Vocal Limiter, Warmth & Body, Vocal Clarity, Breath Control va Vocal Exciter hien la khung gioi thieu; chua co DSP va chua duoc ket noi dieu khien. Khong duoc coi la tinh nang da hoat dong.
- Them cua so Vocal Leveling voi cong tac ON/OFF, Target Level, Maximum Boost va Maximum Reduction.
- Them bo can bang muc am luong theo envelope, gioi han muc tang/cat, smoothing va nguong bao ve doan tin hieu nho de tranh nang tieng on khi hat ngat cau.
- Vocal Leveling chi chay khi BAT XU LY duoc bat, Bypass tat va cong tac Vocal Leveling ON. Khi tat module, gain rider tro ve unity theo smoothing.
- Xu ly realtime dung state co dinh, khong them cap phat bo nho hay khoa trong audio callback.
- Chua build hoac nghe thu tren may nguoi dung. Can Rebuild Solution va thu bang micro that; can kiem tra pumping, tieng on giua cau, do tu nhien va clipping.
