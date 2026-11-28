#pragma once

#include <JuceHeader.h>
#include <memory>
#include <vector>
#include <array>
#include <atomic>

class StudioKnobLookAndFeel : public juce::LookAndFeel_V4
{
public:
    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                          float sliderPosProportional, float rotaryStartAngle,
                          float rotaryEndAngle, juce::Slider& slider) override;
};

class MainComponent : public juce::AudioAppComponent, private juce::Timer
{
public:
    MainComponent();
    ~MainComponent() override;

    void paint(juce::Graphics&) override;
    void resized() override;
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;
    void timerCallback() override;

private:
    struct BiquadFilter
    {
        float b0 { 1.0f }, b1 { 0.0f }, b2 { 0.0f };
        float a1 { 0.0f }, a2 { 0.0f };
        float z1 { 0.0f }, z2 { 0.0f };

        void setCoefficients(float newB0, float newB1, float newB2,
                             float newA1, float newA2) noexcept
        {
            b0 = newB0; b1 = newB1; b2 = newB2;
            a1 = newA1; a2 = newA2;
        }

        float process(float input) noexcept
        {
            const float output = b0 * input + z1;
            z1 = b1 * input - a1 * output + z2;
            z2 = b2 * input - a2 * output;
            return output;
        }

        void reset() noexcept { z1 = z2 = 0.0f; }
    };

    struct PitchShifter
    {
        static constexpr int bufferSize = 8192;
        std::array<float, bufferSize> delayLine {};
        int writePosition { 0 };
        float phase { 0.0f };
        float currentRatio { 1.0f };
        float wetMix { 0.0f };

        void reset() noexcept
        {
            delayLine.fill(0.0f);
            writePosition = 0;
            phase = 0.0f;
            currentRatio = 1.0f;
            wetMix = 0.0f;
        }

        float readDelay(float delaySamples) const noexcept
        {
            float position = (float)writePosition - delaySamples;
            while (position < 0.0f)
                position += (float)bufferSize;
            while (position >= (float)bufferSize)
                position -= (float)bufferSize;

            const int indexA = (int)position;
            const int indexB = (indexA + 1) % bufferSize;
            const float fraction = position - (float)indexA;
            const float a = delayLine[(size_t)indexA];
            const float b = delayLine[(size_t)indexB];
            return a + (b - a) * fraction;
        }

        float process(float input, float targetRatio, float sampleRate,
                      float smoothingMs, bool enabled) noexcept
        {
            delayLine[(size_t)writePosition] = input;

            const float safeRate = juce::jmax(8000.0f, sampleRate);
            targetRatio = juce::jlimit(0.943874f, 1.059463f, targetRatio);
            const float smoothingSeconds = juce::jlimit(0.010f, 0.150f,
                                                        smoothingMs * 0.001f);
            const float ratioCoefficient = std::exp(-1.0f / (safeRate * smoothingSeconds));
            currentRatio = ratioCoefficient * currentRatio
                         + (1.0f - ratioCoefficient) * targetRatio;

            const float minimumDelay = safeRate * 0.003f;
            const float maximumDelay = safeRate * 0.018f;
            const float delayRange = maximumDelay - minimumDelay;
            const bool shiftingUp = currentRatio >= 1.0f;
            const float phaseIncrement = std::abs(1.0f - currentRatio) / delayRange;

            float phaseA = phase;
            float phaseB = phase + 0.5f;
            if (phaseB >= 1.0f)
                phaseB -= 1.0f;

            const float delayA = shiftingUp
                ? maximumDelay - phaseA * delayRange
                : minimumDelay + phaseA * delayRange;
            const float delayB = shiftingUp
                ? maximumDelay - phaseB * delayRange
                : minimumDelay + phaseB * delayRange;

            const float windowA = std::sin(juce::MathConstants<float>::pi * phaseA);
            const float windowB = std::sin(juce::MathConstants<float>::pi * phaseB);
            const float weightA = windowA * windowA;
            const float weightB = windowB * windowB;
            const float shifted = readDelay(delayA) * weightA
                                + readDelay(delayB) * weightB;

            phase += phaseIncrement;
            if (phase >= 1.0f)
                phase -= std::floor(phase);

            const float targetMix = enabled ? 1.0f : 0.0f;
            const float mixCoefficient = std::exp(-1.0f / (safeRate * 0.010f));
            wetMix = mixCoefficient * wetMix + (1.0f - mixCoefficient) * targetMix;
            const float output = input * (1.0f - wetMix) + shifted * wetMix;

            writePosition = (writePosition + 1) % bufferSize;
            return output;
        }
    };

    struct ParameterControl
    {
        juce::Label label;
        StudioKnobLookAndFeel lookAndFeel;
        juce::Slider slider;
    };

    using PanelBounds = std::array<juce::Rectangle<int>, 8>;

    ParameterControl* addParameter(const juce::String& name, double minimum,
                                   double maximum, double initial,
                                   const juce::String& suffix, int group);
    PanelBounds calculatePanelBounds() const;
    void styleToggle(juce::ToggleButton& toggle);
    void showAudioSettings();
    void showPitchCorrectionModule();
    void showVocalLevelingModule();
    void showVocalLimiterModule();
    void showVocalSmoothModule();
    void showWarmthBodyModule();
    void showVocalClarityModule();
    void showBreathControlModule();
    void showVocalExciterModule();
    void showPlannedVocalModule(const juce::String& moduleName);
    void applyBuiltInPreset(int presetId);
    void savePresetToFile(const juce::File& file);
    bool loadPresetFromFile(const juce::File& file);

    juce::Label titleLabel;
    juce::Label subtitleLabel;
    juce::Label deviceLabel;
    juce::ComboBox deviceSelector;
    juce::Label connectionLabel;
    juce::String lastAudioDeviceName;
    juce::Label statusLabel;

    std::array<std::vector<std::unique_ptr<ParameterControl>>, 8> groups;
    std::array<juce::ToggleButton, 8> groupToggles {
        juce::ToggleButton("BAT"), juce::ToggleButton("BAT"),
        juce::ToggleButton("BAT"), juce::ToggleButton("BAT"),
        juce::ToggleButton("BAT"), juce::ToggleButton("BAT"),
        juce::ToggleButton("BAT"), juce::ToggleButton("BAT")
    };

    juce::ToggleButton monitorToggle { "THEO DOI AM THANH: BAT" };
    juce::TextButton processButton { "BAT XU LY" };
    juce::TextButton bypassButton { "BO QUA" };
    juce::TextButton resetButton { "DAT LAI" };
    juce::TextButton settingsButton { "CAI DAT" };
    juce::TextButton pitchCorrectionButton { "PITCH CORRECTION" };
    juce::TextButton vocalLevelingButton { "VOCAL LEVELING" };
    juce::TextButton vocalLimiterButton { "VOCAL LIMITER" };
    juce::TextButton vocalSmoothButton { "VOCAL SMOOTH" };
    juce::TextButton warmthBodyButton { "WARMTH & BODY" };
    juce::TextButton vocalClarityButton { "VOCAL CLARITY" };
    juce::TextButton breathControlButton { "BREATH CONTROL" };
    juce::TextButton vocalExciterButton { "VOCAL EXCITER" };
    juce::TextButton savePresetButton { "LUU CAI DAT" };
    juce::TextButton managePresetButton { "QUAN LY..." };
    juce::ComboBox presetSelector;
    std::unique_ptr<juce::FileChooser> presetFileChooser;
    juce::Slider masterVolumeSlider;
    juce::Label masterVolumeLabel;
    juce::Label masterValueLabel;

    std::array<std::atomic<float>, 5> eqGainDb;
    std::array<std::atomic<bool>, 8> groupEnabled;
    std::array<BiquadFilter, 5> eqFilters;
    std::array<float, 5> lastAppliedEqGainDb {
        999.0f, 999.0f, 999.0f, 999.0f, 999.0f
    };

    std::atomic<float> compressorThresholdDb { -18.0f };
    std::atomic<float> compressorRatio { 3.0f };
    std::atomic<float> compressorAttackMs { 10.0f };
    std::atomic<float> compressorReleaseMs { 150.0f };
    std::atomic<float> compressorGainReductionDb { 0.0f };
    float compressorEnvelope { 0.0f };

    std::atomic<float> deEsserThresholdDb { -30.0f };
    std::atomic<float> deEsserFrequencyHz { 6000.0f };
    float deEsserEnvelope { 0.0f };
    float lastDeEsserFrequencyHz { 0.0f };
    BiquadFilter deEsserDetectorFilter;
    BiquadFilter deEsserBandFilter;

    std::atomic<float> voicePresence { 60.0f };
    std::atomic<float> voiceWarmth { 58.0f };
    std::atomic<float> voiceAir { 60.0f };
    std::array<BiquadFilter, 3> voiceEnhanceFilters;
    std::array<float, 3> lastVoiceEnhanceGainDb { 999.0f, 999.0f, 999.0f };

    std::atomic<float> noiseGateThresholdDb { -50.0f };
    std::atomic<float> noiseGateHoldMs { 200.0f };
    std::atomic<float> noiseGateReleaseMs { 300.0f };
    float noiseGateEnvelope { 0.0f };
    float noiseGateGain { 1.0f };
    int noiseGateHoldCounter { 0 };

    std::atomic<float> toneBassDb { 2.0f };
    std::atomic<float> toneMidDb { 0.0f };
    std::atomic<float> toneTrebleDb { 2.0f };
    std::array<BiquadFilter, 3> toneFilters;
    std::array<float, 3> lastToneGainDb { 999.0f, 999.0f, 999.0f };

    std::atomic<float> reverbMixPercent { 20.0f };
    std::atomic<float> reverbRoomPercent { 50.0f };
    std::atomic<float> reverbTimeSeconds { 1.8f };
    juce::Reverb reverb;
    float lastReverbMixPercent { -1.0f };
    float lastReverbRoomPercent { -1.0f };
    float lastReverbTimeSeconds { -1.0f };
    bool reverbWasActive { false };

    double currentSampleRate { 44100.0 };

    std::atomic<int> pitchSongKey { 0 };
    std::atomic<int> pitchScale { 1 };
    std::atomic<float> pitchCorrectionAmount { 50.0f };
    std::atomic<float> pitchCorrectionSpeed { 45.0f };
    std::atomic<int> pitchDetectedMidi { -1 };
    std::atomic<int> pitchTargetMidi { -1 };
    std::atomic<float> pitchDeviationCents { 0.0f };
    std::atomic<bool> pitchCorrectionEnabled { false };
    std::atomic<bool> vocalLevelingEnabled { false };
    std::atomic<bool> vocalLimiterEnabled { false };
    std::atomic<bool> vocalSmoothEnabled { false };
    std::atomic<float> vocalSmoothSensitivity { 45.0f };
    std::atomic<float> vocalSmoothIntensity { 35.0f };
    BiquadFilter vocalSmoothDetectorFilter;
    BiquadFilter vocalSmoothBandFilter;
    float vocalSmoothEnvelope { 0.0f };
    float vocalSmoothGain { 1.0f };
    std::atomic<float> vocalLimiterCeilingDb { -1.0f };
    std::atomic<float> vocalLimiterReleaseMs { 80.0f };
    float vocalLimiterGain { 1.0f };
    std::atomic<bool> warmthBodyEnabled { false };
    std::atomic<float> warmthAmount { 50.0f };
    std::atomic<float> bodyAmount { 50.0f };
    BiquadFilter warmthFilter;
    BiquadFilter bodyFilter;
    float lastWarmthGainDb { 999.0f };
    float lastBodyGainDb { 999.0f };
    std::atomic<bool> vocalClarityEnabled { false };
    std::atomic<float> clarityAmount { 50.0f };
    std::atomic<float> clarityAir { 50.0f };
    std::atomic<bool> breathControlEnabled { false };
    std::atomic<float> breathSensitivity { 50.0f };
    std::atomic<float> breathReduction { 35.0f };
    BiquadFilter breathDetectorFilter;
    BiquadFilter breathBandFilter;
    float breathDetectorEnvelope { 0.0f };
    float breathVoiceEnvelope { 0.0f };
    float breathReductionGain { 1.0f };
    BiquadFilter clarityFilter;
    BiquadFilter clarityAirFilter;
    std::atomic<bool> vocalExciterEnabled { false };
    std::atomic<float> vocalExciterAmount { 25.0f };
    std::atomic<float> vocalExciterMix { 25.0f };
    BiquadFilter vocalExciterFilter;
    float vocalExciterMixGain { 0.0f };
    float lastClarityGainDb { 999.0f };
    float lastClarityAirGainDb { 999.0f };
    std::atomic<float> vocalLevelTargetDb { -18.0f };
    std::atomic<float> vocalLevelMaxBoostDb { 6.0f };
    std::atomic<float> vocalLevelMaxCutDb { 4.0f };
    float vocalLevelEnvelope { 0.0f };
    float vocalLevelGain { 1.0f };
    PitchShifter pitchShifter;
    std::array<float, 2048> pitchAnalysisBuffer {};
    int pitchAnalysisWritePosition { 0 };
    int pitchAnalysisSamplesSinceUpdate { 0 };

    std::atomic<float> inputGainLinear { 1.0f };
    std::atomic<float> outputGainLinear { 1.0f };
    std::atomic<float> masterVolumeLinear { 1.0f };
    std::atomic<float> inputPeak { 0.0f };
    std::atomic<bool> processingEnabled { false };
    std::atomic<bool> monitorEnabled { true };
    std::atomic<bool> bypassEnabled { false };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};
