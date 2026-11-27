#pragma once

#include <JuceHeader.h>
#include <memory>
#include <vector>
#include <array>
#include <atomic>

class InputVUMeter : public juce::Component
{
public:
    void paint(juce::Graphics& g) override;
    void setLevel(float newLevel) noexcept { level.store(newLevel); }
private:
    std::atomic<float> level { 0.0f };
};

class MicVolumeLookAndFeel : public juce::LookAndFeel_V4
{
public:
    void drawRotarySlider(juce::Graphics& g,
                          int x, int y, int width, int height,
                          float sliderPosProportional,
                          float rotaryStartAngle,
                          float rotaryEndAngle,
                          juce::Slider& slider) override;
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
    struct ParameterControl
    {
        juce::Label label;
        MicVolumeLookAndFeel rotaryLookAndFeel;
        juce::Slider slider;
    };

    ParameterControl* addParameter(const juce::String& name,
                                   double minimum,
                                   double maximum,
                                   double initial,
                                   const juce::String& suffix,
                                   int group);

    juce::Label titleLabel;
    juce::Label inputMeterLabel;
    InputVUMeter inputMeter;
    std::array<juce::Label, 4> groupLabels;
    std::array<std::vector<std::unique_ptr<ParameterControl>>, 4> groups;

    juce::ToggleButton mixerEnabled { "ON" };
    juce::ToggleButton eqEnabled { "ON" };
    juce::ToggleButton compressorEnabled { "ON" };
    juce::ToggleButton cleanupEnabled { "ON" };

    juce::TextButton processButton { "BAT XU LY" };
    juce::TextButton bypassButton { "BYPASS" };
    juce::Label statusLabel;

    std::atomic<float> inputGainLinear { 1.0f };
    std::atomic<float> micGainLinear { 0.75f };
    std::atomic<float> outputGainLinear { 0.80f };
    std::atomic<float> inputPeak { 0.0f };
    std::atomic<bool> processingEnabled { false };
    std::atomic<bool> bypassEnabled { false };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};
