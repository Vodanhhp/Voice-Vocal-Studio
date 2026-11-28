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

    std::atomic<float> inputGainLinear { 1.0f };
    std::atomic<float> outputGainLinear { 1.0f };
    std::atomic<float> masterVolumeLinear { 1.0f };
    std::atomic<float> inputPeak { 0.0f };
    std::atomic<bool> processingEnabled { false };
    std::atomic<bool> monitorEnabled { true };
    std::atomic<bool> bypassEnabled { false };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};
