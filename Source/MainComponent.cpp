#include "MainComponent.h"

namespace
{
    const juce::Colour textColour { 235, 240, 247 };
    const juce::Colour mutedColour { 165, 176, 190 };
    const juce::Colour accentColour { 65, 170, 225 };
    const juce::Colour controlBackground { 35, 43, 56 };
}

void MicVolumeLookAndFeel::drawRotarySlider(
    juce::Graphics& g, int x, int y, int width, int height,
    float sliderPosProportional, float rotaryStartAngle,
    float rotaryEndAngle, juce::Slider& slider)
{
    juce::ignoreUnused(slider);
    const float diameter = (float)juce::jmin(width, height) - 8.0f;
    const float radius = diameter * 0.5f;
    const float centreX = (float)x + (float)width * 0.5f;
    const float centreY = (float)y + radius + 3.0f;
    const float angle = rotaryStartAngle
        + sliderPosProportional * (rotaryEndAngle - rotaryStartAngle);

    // Dark dial body with a clear outer rim.
    g.setColour(juce::Colour(18, 25, 36));
    g.fillEllipse(centreX - radius, centreY - radius, diameter, diameter);
    g.setColour(juce::Colour(105, 126, 148));
    g.drawEllipse(centreX - radius, centreY - radius, diameter, diameter, 2.0f);

    // Coloured value arc.
    juce::Path valueArc;
    valueArc.addCentredArc(centreX, centreY, radius - 4.0f, radius - 4.0f,
                           0.0f, rotaryStartAngle, angle, true);
    g.setColour(juce::Colour(40, 190, 245));
    g.strokePath(valueArc, juce::PathStrokeType(3.5f,
        juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

    // Fixed end markers and a bright pointer.
    for (int i = 0; i <= 10; ++i)
    {
        const float tickAngle = rotaryStartAngle
            + (rotaryEndAngle - rotaryStartAngle) * (float)i / 10.0f;
        const float outer = radius - 1.0f;
        const float inner = radius - (i % 5 == 0 ? 7.0f : 4.0f);
        g.setColour(juce::Colour(175, 190, 207));
        g.drawLine(centreX + std::cos(tickAngle) * inner,
                   centreY + std::sin(tickAngle) * inner,
                   centreX + std::cos(tickAngle) * outer,
                   centreY + std::sin(tickAngle) * outer,
                   i % 5 == 0 ? 1.6f : 1.0f);
    }

    g.setColour(juce::Colour(245, 250, 255));
    g.drawLine(centreX, centreY,
               centreX + std::cos(angle) * (radius - 10.0f),
               centreY + std::sin(angle) * (radius - 10.0f), 2.8f);
    g.setColour(juce::Colour(40, 190, 245));
    g.fillEllipse(centreX - 3.5f, centreY - 3.5f, 7.0f, 7.0f);
}

void InputVUMeter::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();
    g.setColour(juce::Colour(30, 38, 49));
    g.fillRoundedRectangle(bounds, 6.0f);

    const float cx = bounds.getCentreX();
    const float cy = bounds.getBottom() - 7.0f;
    const float rx = bounds.getWidth() * 0.5f - 15.0f;
    const float ry = bounds.getHeight() - 15.0f;

    juce::Path arc;
    arc.addCentredArc(cx, cy, rx, ry, 0.0f, juce::MathConstants<float>::pi,
                      juce::MathConstants<float>::twoPi, true);
    g.setColour(juce::Colour(125, 145, 165));
    g.strokePath(arc, juce::PathStrokeType(1.4f));

    const juce::String labels[] { "-20", "-10", "-7", "-5", "-3", "0", "+3" };
    for (int i = 0; i <= 20; ++i)
    {
        const float angle = juce::MathConstants<float>::pi
            + juce::MathConstants<float>::pi * (float)i / 20.0f;
        const float cs = std::cos(angle);
        const float sn = std::sin(angle);
        const bool major = (i % 3 == 0) || i == 20;
        const float outerX = cx + cs * rx;
        const float outerY = cy + sn * ry;
        const float innerX = cx + cs * (rx - (major ? 8.0f : 4.0f));
        const float innerY = cy + sn * (ry - (major ? 8.0f : 4.0f));
        g.setColour(juce::Colour(190, 202, 215));
        g.drawLine(innerX, innerY, outerX, outerY, major ? 1.4f : 0.8f);
    }

    for (int i = 0; i < 7; ++i)
    {
        const float angle = juce::MathConstants<float>::pi
            + juce::MathConstants<float>::pi * (float)(i * 20 / 6) / 20.0f;
        const float tx = cx + std::cos(angle) * (rx - 17.0f) - 12.0f;
        const float ty = cy + std::sin(angle) * (ry - 17.0f) - 7.0f;
        g.setFont(juce::FontOptions(8.5f));
        g.setColour(juce::Colour(210, 218, 228));
        g.drawText(labels[i], (int)tx, (int)ty, 24, 12, juce::Justification::centred);
    }

    // Static demonstration needle at -10 dB; live audio will drive it later.
    const float needleAngle = juce::MathConstants<float>::pi
        + juce::MathConstants<float>::pi * (10.0f / 23.0f);
    const float nx = cx + std::cos(needleAngle) * (rx - 12.0f);
    const float ny = cy + std::sin(needleAngle) * (ry - 12.0f);
    g.setColour(juce::Colour(255, 85, 75));
    g.drawLine(cx, cy, nx, ny, 2.0f);
    g.fillEllipse(cx - 3.0f, cy - 3.0f, 6.0f, 6.0f);
}

MainComponent::MainComponent()
{
    juce::LookAndFeel::getDefaultLookAndFeel().setDefaultSansSerifTypefaceName("Segoe UI");
    setSize(820, 620);

    titleLabel.setText("VOICE VOCAL STUDIO - BAN DIEU KHIEN CO BAN", juce::dontSendNotification);
    titleLabel.setFont(juce::FontOptions(23.0f, juce::Font::bold));
    titleLabel.setColour(juce::Label::textColourId, accentColour);
    addAndMakeVisible(titleLabel);

    // TEST 07: visual input meter only; it is not connected to live audio yet.
    inputMeterLabel.setText("TEST 07 - VU INPUT | KIM + THANG dB (MO PHONG)", juce::dontSendNotification);
    inputMeterLabel.setFont(juce::FontOptions(10.5f, juce::Font::bold));
    inputMeterLabel.setColour(juce::Label::textColourId, juce::Colour(110, 210, 255));
    addAndMakeVisible(inputMeterLabel);
    addAndMakeVisible(inputMeter);

    infoLabel.setText("Giao dien thu nghiem - cac thong so hien tai la dieu khien giao dien, chua xu ly am thanh",
                      juce::dontSendNotification);

    // TEST 04: visible build marker. Keep this marker in future revisions.
    test04Label.setText("TEST 04 - BUILD MARKER: sliders + layout", juce::dontSendNotification);
    test04Label.setFont(juce::FontOptions(13.0f, juce::Font::bold));
    test04Label.setColour(juce::Label::textColourId, juce::Colour(255, 220, 70));
    addAndMakeVisible(test04Label);
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

    // TEST 05: force a final layout after every child control has been created.
    resized();
    test05Label.setText("TEST 05 - FINAL LAYOUT CALLED", juce::dontSendNotification);
    test05Label.setFont(juce::FontOptions(12.0f, juce::Font::bold));
    test05Label.setColour(juce::Label::textColourId, juce::Colour(110, 235, 150));
    addAndMakeVisible(test05Label);

    // TEST 08: Mic Volume is the first rotary-knob trial; all other controls stay unchanged.\n    // TEST 06: high-contrast LinearBar sliders with an explicit background colour.
    test06Label.setText("TEST 06 - HIGH CONTRAST SLIDER STYLE", juce::dontSendNotification);
    test06Label.setFont(juce::FontOptions(12.0f, juce::Font::bold));
    test06Label.setColour(juce::Label::textColourId, juce::Colour(100, 210, 255));
    addAndMakeVisible(test06Label);
    resized();
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

    const bool isMicVolume = (name == "Mic Volume");
    result->slider.setSliderStyle(isMicVolume
        ? juce::Slider::RotaryHorizontalVerticalDrag
        : juce::Slider::LinearBar);
    result->slider.setTextBoxStyle(isMicVolume
        ? juce::Slider::TextBoxBelow
        : juce::Slider::TextBoxRight,
        false, isMicVolume ? 64 : 65, isMicVolume ? 18 : 24);
    if (isMicVolume)
    {
        result->slider.setLookAndFeel(&result->rotaryLookAndFeel);
        result->slider.setRotaryParameters(
            juce::MathConstants<float>::pi * 1.2f,
            juce::MathConstants<float>::pi * 2.8f,
            true);
        result->slider.setColour(juce::Slider::rotarySliderFillColourId,
                                 juce::Colour(40, 190, 245));
        result->slider.setColour(juce::Slider::rotarySliderOutlineColourId,
                                 juce::Colour(75, 90, 110));
    }
    result->slider.setRange(minimum, maximum, 0.1);
    result->slider.setValue(initial, juce::dontSendNotification);
    result->slider.setTextValueSuffix(suffix);
    result->slider.setColour(juce::Slider::textBoxTextColourId, juce::Colours::white);
    result->slider.setColour(juce::Slider::textBoxBackgroundColourId, juce::Colour(45, 55, 70));
    result->slider.setColour(juce::Slider::textBoxOutlineColourId, juce::Colour(120, 145, 170));
    result->slider.setColour(juce::Slider::backgroundColourId, juce::Colour(55, 65, 80));
    result->slider.setColour(juce::Slider::trackColourId, juce::Colour(40, 190, 245));
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

    titleLabel.setBounds(area.getX(), area.getY(), 505, 32);
    inputMeterLabel.setBounds(area.getRight() - 260, area.getY(), 260, 15);
    inputMeter.setBounds(area.getRight() - 260, area.getY() + 16, 260, 58);
    area.removeFromTop(32);
    infoLabel.setBounds(area.removeFromTop(24));
    test04Label.setBounds(area.removeFromTop(24));
    test05Label.setBounds(area.removeFromTop(22));
    test06Label.setBounds(area.removeFromTop(22));
    area.removeFromTop(4);

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
            if (group == 0 && i == 1) // TEST 08: only Mic Volume becomes a rotary knob.
                control.slider.setBounds(groupArea.getX() + 108, rowY - 8, 88, 64);
            else
                control.slider.setBounds(groupArea.getX() + 108, rowY, groupArea.getWidth() - 108, 28);
        }
    }

    const int bottomY = getHeight() - 62;
    processButton.setBounds(18, bottomY, 135, 30);
    bypassButton.setBounds(162, bottomY, 105, 30);
    statusLabel.setBounds(18, bottomY + 32, getWidth() - 36, 20);
}
