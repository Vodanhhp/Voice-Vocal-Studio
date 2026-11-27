#pragma once

#include <JuceHeader.h>
#include <memory>
#include <vector>
#include <array>

// A rotary control for the first UI prototype.
// Values are editable in the interface but are not connected to DSP yet.
class KnobControl final : public juce::Component
{
public:
    KnobControl(const juce::String& title,
                double minimum,
                double maximum,
                double initial,
                const juce::String& suffix = {});

    void resized() override;
    void paint(juce::Graphics& g) override;

private:
    juce::Label titleLabel;
    juce::Slider slider;
    juce::Label valueLabel;
    juce::String unitSuffix;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(KnobControl)
};

//==============================================================================
class MainComponent : public juce::Component
{
public:
    MainComponent();
    ~MainComponent() override = default;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    KnobControl* addKnob(const juce::String& title,
                         double minimum,
                         double maximum,
                         double initial,
                         const juce::String& suffix,
                         int group);

    void updateStatus(const juce::String& message);

    juce::Label titleLabel;
    juce::Label subtitleLabel;
    juce::Label deviceLabel;
    juce::Label inputLabel;
    juce::Label outputLabel;
    juce::Label engineLabel;
    juce::ComboBox inputDeviceBox;
    juce::ComboBox outputDeviceBox;
    juce::ComboBox presetBox;
    juce::Slider micVolumeSlider;
    juce::Slider outputVolumeSlider;
    juce::Label micVolumeLabel;
    juce::Label outputVolumeLabel;

    juce::Label inputMeterLabel;
    juce::Label outputMeterLabel;
    juce::Label inputMeterState;
    juce::Label outputMeterState;

    std::array<juce::Label, 4> sectionTitles;
    std::array<juce::ToggleButton, 4> sectionToggles;
    std::array<juce::Rectangle<int>, 4> panelBounds;
    std::vector<std::unique_ptr<KnobControl>> knobs;
    std::array<std::vector<KnobControl*>, 4> groupKnobs;

    juce::TextButton processButton { "BAT XU LY" };
    juce::TextButton bypassButton { "BYPASS TOAN BO" };
    juce::Label statusLabel;
    juce::Label footerLabel;

    bool processingRequested = false;
    bool bypassRequested = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};
