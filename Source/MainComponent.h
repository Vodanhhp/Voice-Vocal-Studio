#pragma once

#include <JuceHeader.h>
#include <memory>
#include <vector>
#include <array>

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
        juce::Slider slider;
    };

    ParameterControl* addParameter(const juce::String& name,
                                   double minimum,
                                   double maximum,
                                   double initial,
                                   const juce::String& suffix,
                                   int group);

    juce::Label titleLabel;
    juce::Label infoLabel;
    juce::Label test04Label; // Persistent build marker; retain in future tests.
    juce::Label test05Label; // Persistent build marker; retain in future tests.
    std::array<juce::Label, 4> groupLabels;
    std::array<std::vector<std::unique_ptr<ParameterControl>>, 4> groups;

    juce::ToggleButton mixerEnabled { "Bat Mixer" };
    juce::ToggleButton eqEnabled { "Bat EQ" };
    juce::ToggleButton compressorEnabled { "Bat Compressor" };
    juce::ToggleButton cleanupEnabled { "Bat lam sach giong" };

    juce::TextButton processButton { "BAT XU LY" };
    juce::TextButton bypassButton { "BYPASS" };
    juce::Label statusLabel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};
