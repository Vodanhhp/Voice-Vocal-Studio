
#pragma once

#include <JuceHeader.h>

//==============================================================================
class MainComponent : public juce::Component
{
public:
    MainComponent();
    ~MainComponent() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    // Tiêu đề
    juce::Label titleLabel;
    juce::Label subtitleLabel;

    // Thiết bị âm thanh
    juce::Label deviceSectionLabel;
    juce::Label inputLabel;
    juce::Label outputLabel;
    juce::ComboBox inputDeviceBox;
    juce::ComboBox outputDeviceBox;

    // Mức tín hiệu
    juce::Label meterLabel;
    juce::Label meterStatusLabel;

    // Chế độ
    juce::Label modeSectionLabel;
    juce::TextButton karaokeButton{ "Karaoke" };
    juce::TextButton mcButton{ "MC / Tro chuyen" };

    // Điều khiển
    juce::TextButton processingButton{ "BAT XU LY" };
    juce::TextButton bypassButton{ "BYPASS" };

    juce::Label footerLabel;

    bool isProcessingEnabled = false;
    bool isBypassEnabled = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};
