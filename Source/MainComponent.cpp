#include "MainComponent.h"

namespace
{
    const juce::Colour textColour { 235, 240, 247 };
    const juce::Colour mutedColour { 165, 176, 190 };
    const juce::Colour accentColour { 65, 170, 225 };
    const juce::Colour controlBackground { 35, 43, 56 };
}

MainComponent::MainComponent()
{
    juce::LookAndFeel::getDefaultLookAndFeel().setDefaultSansSerifTypefaceName("Segoe UI");
    setSize(820, 620);

    titleLabel.setText("VOICE VOCAL STUDIO - BAN DIEU KHIEN CO BAN", juce::dontSendNotification);
    titleLabel.setFont(juce::FontOptions(23.0f, juce::Font::bold));
    titleLabel.setColour(juce::Label::textColourId, accentColour);
    addAndMakeVisible(titleLabel);

    infoLabel.setText("Giao dien thu nghiem - cac thong so hien tai la dieu khien giao dien, chua xu ly am thanh",
                      juce::dontSendNotification);
    infoLabel.setFont(juce::FontOptions(11.0f));
    infoLabel.setColour(juce::Label::textColourId, mutedColour);
    addAndMakeVisible(infoLabel);

    const std::array<juce::String, 4> names {
        "MIXER / MUC TIN HIEU", "EQUALIZER / EQ 3 DAI",
        "COMPRESSOR / NEN GIONG", "LAM SACH GIONG"
    };
    for (int i = 0; i < 4; ++i)
    {
        groupLabels[(size_t)i].setText(names[(size_t)i], juce::dontSendNotification);
        groupLabels[(size_t)i].setFont(juce::FontOptions(14.0f, juce::Font::bold));
        groupLabels[(size_t)i].setColour(juce::Label::textColourId, textColour);
        addAndMakeVisible(groupLabels[(size_t)i]);
    }

    mixerEnabled.setToggleState(true, juce::dontSendNotification);
    for (auto* toggle : { &mixerEnabled, &eqEnabled, &compressorEnabled, &cleanupEnabled })
    {
        toggle->setColour(juce::ToggleButton::textColourId, textColour);
        addAndMakeVisible(*toggle);
    }

    addParameter("Input Gain", -12.0, 24.0, 0.0, " dB", 0);
    addParameter("Mic Volume", 0.0, 100.0, 75.0, " %", 0);
    addParameter("Output Volume", 0.0, 100.0, 80.0, " %", 0);

    addParameter("Bass", -12.0, 12.0, 2.0, " dB", 1);
    addParameter("Mid", -12.0, 12.0, 0.0, " dB", 1);
    addParameter("Treble", -12.0, 12.0, 1.0, " dB", 1);

    addParameter("Threshold", -60.0, 0.0, -18.0, " dB", 2);
    addParameter("Ratio", 1.0, 20.0, 3.0, " : 1", 2);

    addParameter("De-esser", 0.0, 100.0, 25.0, " %", 3);
    addParameter("Noise Reduce", 0.0, 100.0, 15.0, " %", 3);
    addParameter("Limiter", -12.0, 0.0, -1.0, " dB", 3);

    processButton.setClickingTogglesState(true);
    processButton.setColour(juce::TextButton::buttonColourId, juce::Colour(26, 132, 99));
    processButton.setColour(juce::TextButton::textColourOffId, juce::Colours::white);
    processButton.setColour(juce::TextButton::textColourOnId, juce::Colours::white);
    processButton.onClick = [this]
    {
        statusLabel.setText(processButton.getToggleState()
            ? "Da bat yeu cau xu ly - chua co DSP engine"
            : "Da tat yeu cau xu ly", juce::dontSendNotification);
    };
    addAndMakeVisible(processButton);

    bypassButton.setClickingTogglesState(true);
    bypassButton.setColour(juce::TextButton::buttonColourId, juce::Colour(91, 99, 113));
    bypassButton.setColour(juce::TextButton::textColourOffId, juce::Colours::white);
    bypassButton.setColour(juce::TextButton::textColourOnId, juce::Colours::white);
    bypassButton.onClick = [this]
    {
        statusLabel.setText(bypassButton.getToggleState()
            ? "Bypass dang bat - chua co DSP engine"
            : "Bypass da tat", juce::dontSendNotification);
    };
    addAndMakeVisible(bypassButton);

    statusLabel.setText("San sang - cac thanh truot va o so co the thao tac",
                        juce::dontSendNotification);
    statusLabel.setFont(juce::FontOptions(11.5f, juce::Font::bold));
    statusLabel.setColour(juce::Label::textColourId, juce::Colour(242, 183, 91));
    addAndMakeVisible(statusLabel);
}

MainComponent::ParameterControl* MainComponent::addParameter(
    const juce::String& name, double minimum, double maximum, double initial,
    const juce::String& suffix, int group)
{
    auto control = std::make_unique<ParameterControl>();
    auto* result = control.get();

    result->label.setText(name, juce::dontSendNotification);
    result->label.setFont(juce::FontOptions(11.5f, juce::Font::bold));
    result->label.setColour(juce::Label::textColourId, textColour);
    addAndMakeVisible(result->label);

    result->slider.setSliderStyle(juce::Slider::LinearHorizontal);
    result->slider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 65, 24);
    result->slider.setRange(minimum, maximum, 0.1);
    result->slider.setValue(initial, juce::dontSendNotification);
    result->slider.setTextValueSuffix(suffix);
    result->slider.setColour(juce::Slider::textBoxTextColourId, textColour);
    result->slider.setColour(juce::Slider::textBoxBackgroundColourId, controlBackground);
    result->slider.setColour(juce::Slider::textBoxOutlineColourId, juce::Colour(80, 91, 108));
    result->slider.setColour(juce::Slider::trackColourId, accentColour);
    result->slider.setColour(juce::Slider::thumbColourId, juce::Colours::white);
    addAndMakeVisible(result->slider);

    groups[(size_t)group].push_back(std::move(control));
    return result;
}

void MainComponent::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(22, 27, 36));
}

void MainComponent::resized()
{
    auto area = getLocalBounds().reduced(18);

    titleLabel.setBounds(area.removeFromTop(32));
    infoLabel.setBounds(area.removeFromTop(24));
    area.removeFromTop(8);

    const int gap = 18;
    const int columnWidth = (area.getWidth() - gap) / 2;
    const int groupHeight = 205;

    for (int group = 0; group < 4; ++group)
    {
        const int column = group % 2;
        const int row = group / 2;
        const int x = area.getX() + column * (columnWidth + gap);
        const int y = area.getY() + row * (groupHeight + 10);
        auto groupArea = juce::Rectangle<int>(x, y, columnWidth, groupHeight);

        groupLabels[(size_t)group].setBounds(groupArea.getX(), groupArea.getY(), groupArea.getWidth() - 112, 24);
        juce::ToggleButton* toggles[] = { &mixerEnabled, &eqEnabled, &compressorEnabled, &cleanupEnabled };
        toggles[group]->setBounds(groupArea.getRight() - 108, groupArea.getY(), 108, 22);
        groupArea.removeFromTop(32);

        const int rowHeight = 48;
        for (size_t i = 0; i < groups[(size_t)group].size(); ++i)
        {
            auto& control = *groups[(size_t)group][i];
            const int rowY = groupArea.getY() + (int)i * rowHeight;
            control.label.setBounds(groupArea.getX(), rowY, 105, 26);
            control.slider.setBounds(groupArea.getX() + 108, rowY, groupArea.getWidth() - 108, 28);
        }
    }

    const int bottomY = getHeight() - 62;
    processButton.setBounds(18, bottomY, 135, 30);
    bypassButton.setBounds(162, bottomY, 105, 30);
    statusLabel.setBounds(18, bottomY + 32, getWidth() - 36, 20);
}
