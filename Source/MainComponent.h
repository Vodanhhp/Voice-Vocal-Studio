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

    juce::Label titleLabel;
    juce::Label subtitleLabel;
    juce::Label deviceLabel;
    juce::ComboBox deviceSelector;
    juce::Label connectionLabel;
    juce::String lastAudioDeviceName;
    juce::Label statusLabel;

    std::array<std::vector<std::unique_ptr<ParameterControl>>, 8> groups;
    std::array<juce::ToggleButton, 8> groupToggles {
        juce::ToggleButton("ON"), juce::ToggleButton("ON"),
        juce::ToggleButton("ON"), juce::ToggleButton("ON"),
        juce::ToggleButton("ON"), juce::ToggleButton("ON"),
        juce::ToggleButton("ON"), juce::ToggleButton("ON")
    };

    juce::ToggleButton monitorToggle { "ON" };
    juce::TextButton processButton { "BAT XU LY" };
    juce::TextButton bypassButton { "BYPASS" };
    juce::TextButton resetButton { "RESET" };
    juce::TextButton settingsButton { "CAI DAT" };
    juce::TextButton savePresetButton { "LUU PRESET" };
    juce::TextButton managePresetButton { "QUAN LY..." };
    juce::ComboBox presetSelector;
    juce::Slider masterVolumeSlider;
    juce::Label masterVolumeLabel;
    juce::Label masterValueLabel;

    std::atomic<float> inputGainLinear { 1.0f };
    std::atomic<float> outputGainLinear { 0.8f };
    std::atomic<float> inputPeak { 0.0f };
    std::atomic<bool> processingEnabled { false };
    std::atomic<bool> bypassEnabled { false };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};
