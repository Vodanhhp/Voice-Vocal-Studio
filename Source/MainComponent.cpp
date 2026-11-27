#include "MainComponent.h"

namespace
{
    const juce::Colour backgroundColour { 17, 23, 33 };
    const juce::Colour panelColour { 27, 36, 49 };
    const juce::Colour panelEdge { 54, 69, 87 };
    const juce::Colour textColour { 235, 241, 248 };
    const juce::Colour mutedColour { 145, 160, 178 };
    const juce::Colour accentColour { 50, 185, 235 };
}

void MicVolumeLookAndFeel::drawRotarySlider(
    juce::Graphics& g, int x, int y, int width, int height,
    float sliderPosProportional, float rotaryStartAngle,
    float rotaryEndAngle, juce::Slider& slider)
{
    juce::ignoreUnused(slider);
    const float diameter = (float)juce::jmin(width, height) - 10.0f;
    const float radius = diameter * 0.5f;
    const float cx = (float)x + (float)width * 0.5f;
    const float cy = (float)y + radius + 3.0f;
    const float angle = rotaryStartAngle
        + sliderPosProportional * (rotaryEndAngle - rotaryStartAngle);

    g.setColour(juce::Colour(13, 20, 30));
    g.fillEllipse(cx - radius, cy - radius, diameter, diameter);
    g.setColour(juce::Colour(92, 116, 140));
    g.drawEllipse(cx - radius, cy - radius, diameter, diameter, 2.0f);

    juce::Path arc;
    arc.addCentredArc(cx, cy, radius - 4.0f, radius - 4.0f, 0.0f,
                      rotaryStartAngle, angle, true);
    g.setColour(accentColour);
    g.strokePath(arc, juce::PathStrokeType(4.0f,
        juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

    for (int i = 0; i <= 10; ++i)
    {
        const float a = rotaryStartAngle
            + (rotaryEndAngle - rotaryStartAngle) * (float)i / 10.0f;
        const float outer = radius - 1.0f;
        const float inner = radius - (i % 5 == 0 ? 8.0f : 4.0f);
        g.setColour(juce::Colour(185, 198, 212));
        g.drawLine(cx + std::cos(a) * inner, cy + std::sin(a) * inner,
                   cx + std::cos(a) * outer, cy + std::sin(a) * outer,
                   i % 5 == 0 ? 1.7f : 1.0f);
    }

    g.setColour(juce::Colours::white);
    g.drawLine(cx, cy, cx + std::cos(angle) * (radius - 11.0f),
               cy + std::sin(angle) * (radius - 11.0f), 3.0f);
    g.setColour(accentColour);
    g.fillEllipse(cx - 4.0f, cy - 4.0f, 8.0f, 8.0f);
}

void InputVUMeter::paint(juce::Graphics& g)
{
    auto b = getLocalBounds().toFloat();
    g.setColour(juce::Colour(13, 20, 30));
    g.fillRoundedRectangle(b, 7.0f);

    const float cx = b.getCentreX();
    const float cy = b.getBottom() - 5.0f;
    const float rx = b.getWidth() * 0.5f - 13.0f;
    const float ry = b.getHeight() - 12.0f;

    juce::Path arc;
    arc.addCentredArc(cx, cy, rx, ry, 0.0f,
                      juce::MathConstants<float>::pi,
                      juce::MathConstants<float>::twoPi, true);
    g.setColour(juce::Colour(115, 139, 162));
    g.strokePath(arc, juce::PathStrokeType(1.3f));

    const juce::String labels[] { "-20", "-10", "-7", "-5", "-3", "0", "+3" };
    for (int i = 0; i <= 20; ++i)
    {
        const float a = juce::MathConstants<float>::pi
            + juce::MathConstants<float>::pi * (float)i / 20.0f;
        const bool major = (i % 3 == 0) || i == 20;
        g.setColour(juce::Colour(190, 202, 215));
        g.drawLine(cx + std::cos(a) * (rx - (major ? 8.0f : 4.0f)),
                   cy + std::sin(a) * (ry - (major ? 8.0f : 4.0f)),
                   cx + std::cos(a) * rx, cy + std::sin(a) * ry,
                   major ? 1.3f : 0.8f);
    }

    for (int i = 0; i < 7; ++i)
    {
        const float a = juce::MathConstants<float>::pi
            + juce::MathConstants<float>::pi * (float)(i * 20 / 6) / 20.0f;
        g.setFont(juce::FontOptions(8.0f));
        g.setColour(juce::Colour(205, 216, 228));
        g.drawText(labels[i],
                   (int)(cx + std::cos(a) * (rx - 16.0f) - 11.0f),
                   (int)(cy + std::sin(a) * (ry - 15.0f) - 6.0f),
                   22, 12, juce::Justification::centred);
    }

    // Static visual reference only; not connected to microphone audio yet.
    const float needleAngle = juce::MathConstants<float>::pi
        + juce::MathConstants<float>::pi * (10.0f / 23.0f);
    g.setColour(juce::Colour(255, 82, 72));
    g.drawLine(cx, cy, cx + std::cos(needleAngle) * (rx - 11.0f),
               cy + std::sin(needleAngle) * (ry - 10.0f), 2.0f);
    g.fillEllipse(cx - 3.0f, cy - 3.0f, 6.0f, 6.0f);
}

MainComponent::MainComponent()
{
    juce::LookAndFeel::getDefaultLookAndFeel().setDefaultSansSerifTypefaceName("Segoe UI");
    setSize(760, 560);

    titleLabel.setText("VOICE VOCAL STUDIO", juce::dontSendNotification);
    titleLabel.setFont(juce::FontOptions(22.0f, juce::Font::bold));
    titleLabel.setColour(juce::Label::textColourId, accentColour);
    addAndMakeVisible(titleLabel);

    inputMeterLabel.setText("INPUT LEVEL", juce::dontSendNotification);
    inputMeterLabel.setFont(juce::FontOptions(9.5f, juce::Font::bold));
    inputMeterLabel.setColour(juce::Label::textColourId, mutedColour);
    addAndMakeVisible(inputMeterLabel);
    addAndMakeVisible(inputMeter);

    const std::array<juce::String, 4> names {
        "MIXER", "EQUALIZER", "COMPRESSOR", "VOICE CLEANUP"
    };
    for (int i = 0; i < 4; ++i)
    {
        groupLabels[(size_t)i].setText(names[(size_t)i], juce::dontSendNotification);
        groupLabels[(size_t)i].setFont(juce::FontOptions(13.0f, juce::Font::bold));
        groupLabels[(size_t)i].setColour(juce::Label::textColourId, textColour);
        addAndMakeVisible(groupLabels[(size_t)i]);
    }

    mixerEnabled.setToggleState(true, juce::dontSendNotification);
    for (auto* toggle : { &mixerEnabled, &eqEnabled, &compressorEnabled, &cleanupEnabled })
    {
        toggle->setColour(juce::ToggleButton::textColourId, accentColour);
        toggle->setColour(juce::ToggleButton::tickColourId, accentColour);
        toggle->setColour(juce::ToggleButton::tickDisabledColourId, mutedColour);
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
    processButton.setColour(juce::TextButton::buttonColourId, juce::Colour(24, 145, 105));
    processButton.setColour(juce::TextButton::buttonOnColourId, juce::Colour(18, 112, 83));
    processButton.setColour(juce::TextButton::textColourOffId, juce::Colours::white);
    processButton.setColour(juce::TextButton::textColourOnId, juce::Colours::white);
    processButton.onClick = [this]
    {
        statusLabel.setText(processButton.getToggleState()
            ? "Xu ly: chua ket noi engine am thanh"
            : "Xu ly da tat", juce::dontSendNotification);
    };
    addAndMakeVisible(processButton);

    bypassButton.setClickingTogglesState(true);
    bypassButton.setColour(juce::TextButton::buttonColourId, juce::Colour(65, 78, 96));
    bypassButton.setColour(juce::TextButton::buttonOnColourId, juce::Colour(87, 101, 120));
    bypassButton.setColour(juce::TextButton::textColourOffId, juce::Colours::white);
    bypassButton.setColour(juce::TextButton::textColourOnId, juce::Colours::white);
    bypassButton.onClick = [this]
    {
        statusLabel.setText(bypassButton.getToggleState()
            ? "Bypass: chua ket noi engine am thanh"
            : "Bypass da tat", juce::dontSendNotification);
    };
    addAndMakeVisible(bypassButton);

    statusLabel.setText("San sang", juce::dontSendNotification);
    statusLabel.setFont(juce::FontOptions(10.5f));
    statusLabel.setColour(juce::Label::textColourId, mutedColour);
    addAndMakeVisible(statusLabel);

    resized();
}

MainComponent::ParameterControl* MainComponent::addParameter(
    const juce::String& name, double minimum, double maximum, double initial,
    const juce::String& suffix, int group)
{
    // Keep the group index inside the fixed-size groups array.
    if (group < 0 || group >= (int)groups.size())
        return nullptr;

    auto control = std::make_unique<ParameterControl>();
    auto* result = control.get();

    result->label.setText(name, juce::dontSendNotification);
    result->label.setFont(juce::FontOptions(10.5f, juce::Font::bold));
    result->label.setColour(juce::Label::textColourId, textColour);
    addAndMakeVisible(result->label);

    const bool isMicVolume = (name == "Mic Volume");
    result->slider.setSliderStyle(isMicVolume
        ? juce::Slider::RotaryHorizontalVerticalDrag
        : juce::Slider::LinearBar);
    result->slider.setTextBoxStyle(isMicVolume
        ? juce::Slider::TextBoxBelow
        : juce::Slider::TextBoxRight,
        false, isMicVolume ? 70 : 62, isMicVolume ? 18 : 21);

    if (isMicVolume)
    {
        result->slider.setLookAndFeel(&result->rotaryLookAndFeel);
        result->slider.setRotaryParameters(
            juce::MathConstants<float>::pi * 1.2f,
            juce::MathConstants<float>::pi * 2.8f, true);
    }

    result->slider.setRange(minimum, maximum, 0.1);
    result->slider.setValue(initial, juce::dontSendNotification);
    result->slider.setTextValueSuffix(suffix);
    result->slider.setColour(juce::Slider::textBoxTextColourId, juce::Colours::white);
    result->slider.setColour(juce::Slider::textBoxBackgroundColourId, juce::Colour(18, 27, 39));
    result->slider.setColour(juce::Slider::textBoxOutlineColourId, panelEdge);
    result->slider.setColour(juce::Slider::backgroundColourId, juce::Colour(14, 21, 31));
    result->slider.setColour(juce::Slider::trackColourId, accentColour);
    result->slider.setColour(juce::Slider::thumbColourId, juce::Colours::white);
    addAndMakeVisible(result->slider);

    groups[(size_t)group].push_back(std::move(control));
    return result;
}

void MainComponent::paint(juce::Graphics& g)
{
    g.fillAll(backgroundColour);

    auto area = getLocalBounds().reduced(14);
    area.removeFromTop(66);
    area.removeFromBottom(48);
    const int gap = 12;
    const int columnWidth = (area.getWidth() - gap) / 2;
    const int groupHeight = (area.getHeight() - gap) / 2;

    for (int i = 0; i < 4; ++i)
    {
        const int col = i % 2;
        const int row = i / 2;
        juce::Rectangle<float> card(
            (float)(area.getX() + col * (columnWidth + gap)),
            (float)(area.getY() + row * (groupHeight + gap)),
            (float)columnWidth, (float)groupHeight);
        g.setColour(panelColour);
        g.fillRoundedRectangle(card, 9.0f);
        g.setColour(panelEdge);
        g.drawRoundedRectangle(card, 9.0f, 1.0f);
        g.setColour(accentColour.withAlpha(0.65f));
        g.fillRoundedRectangle(card.getX() + 10.0f, card.getY() + 11.0f,
                                3.0f, 18.0f, 1.5f);
    }
}

void MainComponent::resized()
{
    auto area = getLocalBounds().reduced(14);

    titleLabel.setBounds(area.getX(), area.getY() + 4, 390, 34);
    inputMeterLabel.setBounds(area.getRight() - 218, area.getY(), 218, 14);
    inputMeter.setBounds(area.getRight() - 218, area.getY() + 15, 218, 47);

    area.removeFromTop(66);
    area.removeFromBottom(48);

    const int gap = 12;
    const int columnWidth = (area.getWidth() - gap) / 2;
    const int groupHeight = (area.getHeight() - gap) / 2;
    juce::ToggleButton* toggles[] = {
        &mixerEnabled, &eqEnabled, &compressorEnabled, &cleanupEnabled
    };

    // JUCE may call resized() while addParameter() is still creating controls.
    // Do not access any group's vector until every group is fully populated.
    const bool parametersReady =
        groups[0].size() == 3 &&
        groups[1].size() == 3 &&
        groups[2].size() == 2 &&
        groups[3].size() == 3;

    for (int group = 0; group < 4; ++group)
    {
        const int col = group % 2;
        const int row = group / 2;
        juce::Rectangle<int> card(
            area.getX() + col * (columnWidth + gap),
            area.getY() + row * (groupHeight + gap),
            columnWidth, groupHeight);

        groupLabels[(size_t)group].setBounds(
            card.getX() + 14, card.getY() + 8, card.getWidth() - 100, 26);
        toggles[group]->setBounds(card.getRight() - 72, card.getY() + 8, 62, 24);

        // Header and switches can be laid out immediately; parameter controls wait.
        if (!parametersReady)
            continue;

        const int contentY = card.getY() + 39;
        const int labelX = card.getX() + 14;
        const int sliderX = card.getX() + 116;
        const int sliderWidth = card.getWidth() - 130;

        if (group == 0)
        {
            auto& input = *groups[0][0];
            auto& mic = *groups[0][1];
            auto& output = *groups[0][2];

            input.label.setBounds(labelX, contentY + 7, 96, 22);
            input.slider.setBounds(sliderX, contentY + 3, sliderWidth, 25);

            mic.label.setBounds(labelX, contentY + 49, 96, 24);
            mic.slider.setBounds(sliderX + 8, contentY + 35, 96, 78);

            output.label.setBounds(labelX, contentY + 124, 96, 22);
            output.slider.setBounds(sliderX, contentY + 121, sliderWidth, 25);
        }
        else
        {
            for (size_t i = 0; i < groups[(size_t)group].size(); ++i)
            {
                auto& control = *groups[(size_t)group][i];
                const int rowY = contentY + 12 + (int)i * 43;
                control.label.setBounds(labelX, rowY + 2, 96, 22);
                control.slider.setBounds(sliderX, rowY, sliderWidth, 25);
            }
        }
    }

    const int footerY = getHeight() - 43;
    processButton.setBounds(14, footerY, 122, 30);
    bypassButton.setBounds(144, footerY, 92, 30);
    statusLabel.setBounds(250, footerY + 4, getWidth() - 264, 22);
}
