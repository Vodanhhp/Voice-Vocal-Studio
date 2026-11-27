#pragma once

#include <JuceHeader.h>
#include <memory>
#include <vector>
#include <array>

class InputVUMeter : public juce::Component
{
public:
    void paint(juce::Graphics& g) override;
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

class MainComponent : public juce::Component
{
public:
    MainComponent();
    ~MainComponent() override = default;

    void paint(juce::Graphics&) override;
    void resized() override;

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

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};
