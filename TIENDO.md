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
- Moi nut mo mot cua so rieng. Warmth & Body, Vocal Clarity, Breath Control va Vocal Exciter da duoc them UI va DSP prototype; Vocal Limiter van chua duoc trien khai DSP. Khong coi module nao la hoan tat san pham truoc khi build va nghe thu tren may dich.
- Them cua so Vocal Leveling voi cong tac ON/OFF, Target Level, Maximum Boost va Maximum Reduction.
- Them bo can bang muc am luong theo envelope, gioi han muc tang/cat, smoothing va nguong bao ve doan tin hieu nho de tranh nang tieng on khi hat ngat cau.
- Vocal Leveling chi chay khi BAT XU LY duoc bat, Bypass tat va cong tac Vocal Leveling ON. Khi tat module, gain rider tro ve unity theo smoothing.
- Xu ly realtime dung state co dinh, khong them cap phat bo nho hay khoa trong audio callback.
- Chua build hoac nghe thu tren may nguoi dung. Can Rebuild Solution va thu bang micro that; can kiem tra pumping, tieng on giua cau, do tu nhien va clipping.

## 10. Dinh huong dau ra am thanh ao - 28/11/2026

### Yeu cau da thong nhat
- Luong chinh: Microphone / sound card input -> Voice Vocal Studio DSP -> loa/tai nghe de monitor.
- Dong thoi can co duong gui chinh tin hieu da xu ly sang phan mem thu am.
- Dung virtual audio device (uu tien VB-CABLE) lam dau vao cho phan mem thu am.
- Muc tieu la phan mem thu am co the chon thiet bi ao lam recording input; khong tuyen bo rang moi phan mem tu dong nhan ma khong can chon thiet bi.

### Phan biet driver va EXE
- VB-CABLE la driver cap he thong Windows, phai cai rieng tren may dich; khong duoc dong goi hoac tu y cai driver vao bo cai Voice Vocal Studio.
- Voice Vocal Studio chi chon va mo endpoint audio da duoc Windows cai dat.
- Neu chi chon VB-CABLE lam output chinh thi am thanh se vao cable; can them mot duong monitor rieng de nguoi hat nghe qua loa/tai nghe dong thoi.

### Lo trinh ky thuat
1. Kiem tra va hoan thien viec liet ke/chon input va output endpoint trong Audio Settings.
2. Thiet ke hai dau ra dong thoi: virtual recording output va hardware monitor output. Khong duoc chi copy cung mot AudioIODevice neu driver/device khong ho tro; phai xu ly khac biet sample rate, channel count va buffer.
3. Dung buffer co kich thuoc co dinh / co che thread-safe phu hop giua audio callback; khong cap phat bo nho, khong khoa trong callback realtime.
4. Them trang thai ro rang: chua cai VB-CABLE, da san sang, mat ket noi, hoac khong mo duoc endpoint.
5. Kiem thu tren may dich voi micro that, VB-CABLE, loa/tai nghe va it nhat mot phan mem thu am. Xac nhan file thu co EQ/DSP/Reverb va khong bi lap am, mat tieng, clipping hoac dropouts.

### Trang thai hien tai
- JUCE AudioDeviceSelectorComponent da ton tai trong cua so Cai dat thiet bi am thanh va co the chon audio device theo kha nang driver.
- Chua co duong xuat dong thoi rieng toi VB-CABLE va loa/tai nghe.
- Chua co phat hien / kiem tra VB-CABLE rieng, chua co UI trang thai virtual output.
- Chua duoc kiem thu voi VB-CABLE; chua the coi tinh nang thu am qua thiet bi ao la hoan thanh.
- Khong thay doi hoac dong goi driver ben thu ba trong giai doan nay.



### Implementation update 28/11/2026 - Warmth & Body module (DSP stage 1)
- Replaced the Warmth & Body placeholder window with a dedicated module window containing independent ON/OFF, Warmth and Body controls.
- Connected the controls to atomic realtime parameters consumed by the audio callback.
- Added a low-shelf stage centered around 145 Hz for controlled warmth and a peaking stage around 230 Hz for vocal body.
- Both controls use a neutral midpoint of 50%; their ranges are limited to approximately +/-4 dB and +/-3 dB respectively to reduce the risk of excessive bass buildup.
- The module is active only when the global processing chain is enabled, Bypass is off, and Warmth & Body is ON.
- Filter state is preallocated and processed in the existing audio callback; no new allocations or blocking locks were introduced there.
- This is an initial tonal-shaping implementation, not automatic room/mic correction. Listening tests are required to assess muddiness, proximity effect, microphone response and interaction with existing EQ/Voice Enhance/Tone controls.
- Build and listening tests on the user's computer are pending. Do not mark the module as confirmed working until the user rebuilds and tests with a real microphone.


### Implementation update 28/11/2026 - Vocal Clarity module (DSP stage 2)
- Replaced the Vocal Clarity placeholder with a dedicated module window containing independent ON/OFF, Clarity and Air controls.
- Connected both controls to atomic realtime parameters read by the audio callback.
- Clarity uses a moderate peaking EQ centered near 3.2 kHz to improve articulation and vocal presence.
- Air uses a high-shelf stage near 8.5 kHz for controlled upper-frequency openness.
- Both controls are neutral at 50%; their ranges are limited to approximately +/-3 dB to reduce harshness and sibilance.
- The module is active only when the global processing chain is enabled, Bypass is off, and Vocal Clarity is ON.
- Filter coefficients are updated outside the per-sample loop when control values change; filter state is preallocated and no new callback allocations or blocking locks were introduced.
- Build and listening tests on the user's computer are pending. Listen for intelligibility, harsh consonants, excessive S sounds and interaction with De-Esser, Voice Enhance and Tone Control.


### Implementation update 28/11/2026 - Breath Control module (DSP stage 3)
- Replaced the Breath Control placeholder window with a dedicated module window containing independent ON/OFF, Sensitivity and Breath Reduction controls.
- Connected the controls to atomic realtime parameters consumed by the audio callback.
- Added a fixed 5.5 kHz band-pass detector and a separate band signal path. The module estimates high-frequency breath energy and compares it with the overall vocal envelope before applying attenuation.
- Reduction is applied primarily to the detected high-frequency band rather than turning down the entire vocal signal, helping preserve the main voice.
- Added smoothed detector/envelope and gain behavior to reduce abrupt changes. The fixed filters are configured in prepareToPlay; no new allocations or blocking locks were added to the audio callback.
- The module runs only when global processing is enabled, Bypass is off, and Breath Control is ON.
- This is an initial breath-noise reduction stage, not a source-separation system. It may also affect quiet consonants, soft singing and vocal air; Sensitivity and Reduction need listening tests with the user's microphone.
- Build and listening tests on the user's computer are pending. Start with Sensitivity around 40-50% and Breath Reduction around 20-30%, then compare ON/OFF on breaths between phrases and quiet sung words.


### Implementation update 28/11/2026 - Vocal Exciter module (DSP stage 4)
- Replaced the Vocal Exciter placeholder with a dedicated window containing independent ON/OFF, Exciter Amount and Effect Mix controls.
- Added a fixed band-pass stage centered near 4.8 kHz to isolate upper vocal harmonics before nonlinear saturation.
- Added a bounded tanh waveshaping stage to generate controlled upper harmonics; Amount controls drive and Mix controls the added harmonic level.
- The wet contribution is smoothed over approximately 25 ms to reduce abrupt switching artifacts. The filter is configured in prepareToPlay and uses preallocated state; no new allocations or blocking locks were added to the audio callback.
- The module is active only when global processing is enabled, Bypass is off, and Vocal Exciter is ON.
- This is a subtle harmonic-excitation prototype, not a spectral enhancer or automatic de-esser. Excessive settings can emphasize hiss, S sounds and microphone noise; evaluate with the actual microphone and compare ON/OFF.
- Build and listening tests on the user's computer are pending. Start around Amount 20-30% and Effect Mix 15-25%, then raise gradually only if the vocal needs more brightness.


## 11. Trang thai cong viec - cap nhat sau khi nguoi dung thu Breath Control (28/11/2026)

### Da co phan xu ly DSP trong ma nguon
- Vocal Leveling: da co UI va DSP trong audio callback; can tiep tuc kiem tra thuc te neu chua co ket qua nghe thu day du.
- Warmth & Body: da co UI va DSP; nguoi dung da nghe thu bang micro tai nghe va bao am thanh co cai thien.
- Vocal Clarity: da co UI va DSP; nguoi dung xac nhan chuc nang hoat dong.
- Breath Control: da co UI va DSP; nguoi dung xac nhan module hoat dong va nghe thay mot so thay doi nhe. Hieu qua giam hoi tho chua duoc danh gia day du.
- Vocal Exciter: da viet UI va DSP prototype tren main, nhung chua build thanh cong va chua xac nhan hieu ung hoat dong tren may nguoi dung. Khong danh dau la module da hoat dong.

### Chua trien khai
- Vocal Limiter: nut rieng da co tren giao dien chinh, nhung chua co module DSP thuc su. Khong duoc nham lan nut giao dien/khung module voi tinh nang da hoat dong. Day la module tiep theo can lam sau khi nguoi dung kiem tra Vocal Exciter hoac khi nguoi dung yeu cau tiep tuc.

### Viec can lam tiep theo
1. Nguoi dung Pull main, Rebuild Solution va kiem tra Vocal Exciter bang micro that; so sanh ON/OFF, bat dau Amount 20-30%, Effect Mix 15-25%.
2. Sau khi co ket qua Exciter, trien khai Vocal Limiter: peak detection, adjustable ceiling/threshold, fast attack va controlled release, gain smoothing; tranh clipping ma khong lam giong bi bop/nghet.
3. Kiem tra build va nghe thu tung module tren may dich. Chi danh dau hoan tat sau khi nguoi dung xac nhan.
