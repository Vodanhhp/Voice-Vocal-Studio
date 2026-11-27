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

    // Live input level, refreshed by the message-thread timer.
    const float liveLevel = juce::jlimit(0.0f, 1.0f, level.load());
    const float needleAngle = juce::MathConstants<float>::pi
        + juce::MathConstants<float>::pi * liveLevel;
    g.setColour(juce::Colour(255, 82, 72));
    g.drawLine(cx, cy, cx + std::cos(needleAngle) * (rx - 11.0f),
               cy + std::sin(needleAngle) * (ry - 10.0f), 2.0f);
    g.fillEllipse(cx - 3.0f, cy - 3.0f, 6.0f, 6.0f);
}

MainComponent::MainComponent()
{
    juce::LookAndFeel::getDefaultLookAndFeel().setDefaultSansSerifTypefaceName("Segoe UI");
    setSize(700, 500);

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
    processButton.onClick = [this]\n    {\n        processingEnabled.store(processButton.getToggleState());\n        statusLabel.setText(processButton.getToggleState()\n            ? "Monitor dang chay - tin hieu microphone"\n            : "Monitor da tat", juce::dontSendNotification);\n    };
    addAndMakeVisible(processButton);

    bypassButton.setClickingTogglesState(true);
    bypassButton.setColour(juce::TextButton::buttonColourId, juce::Colour(65, 78, 96));
    bypassButton.setColour(juce::TextButton::buttonOnColourId, juce::Colour(87, 101, 120));
    bypassButton.setColour(juce::TextButton::textColourOffId, juce::Colours::white);
    bypassButton.setColour(juce::TextButton::textColourOnId, juce::Colours::white);
    bypassButton.onClick = [this]\n    {\n        bypassEnabled.store(bypassButton.getToggleState());\n        statusLabel.setText(bypassButton.getToggleState()\n            ? "BYPASS - tin hieu truc tiep"\n            : "Bypass da tat", juce::dontSendNotification);\n    };
    addAndMakeVisible(bypassButton);

    statusLabel.setText("San sang", juce::dontSendNotification);
    statusLabel.setFont(juce::FontOptions(10.5f));
    statusLabel.setColour(juce::Label::textColourId, mutedColour);
    addAndMakeVisible(statusLabel);

    resized();

    // Open the default Windows audio input and stereo output.
    const auto audioError = setAudioChannels(1, 2);
    if (audioError.isNotEmpty())
        statusLabel.setText("Audio: " + audioError, juce::dontSendNotification);
    else
        statusLabel.setText("Audio ready - press BAT XU LY", juce::dontSendNotification);

    startTimerHz(30);
}

MainComponent::~MainComponent()
{
    stopTimer();
    shutdownAudio();
}

void MainComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    juce::ignoreUnused(samplesPerBlockExpected, sampleRate);
}

void MainComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    if (bufferToFill.buffer == nullptr || bufferToFill.numSamples <= 0)
        return;

    auto& buffer = *bufferToFill.buffer;
    const int start = bufferToFill.startSample;
    const int samples = bufferToFill.numSamples;
    const int channels = buffer.getNumChannels();

    if (channels <= 0)
        return;

    if (!processingEnabled.load() && !bypassEnabled.load())
    {
        buffer.clear(start, samples);
        inputPeak.store(0.0f);
        return;
    }

    // AudioAppComponent provides the input in the first channel(s).
    const float* input = buffer.getReadPointer(0, start);
    float peak = 0.0f;
    for (int i = 0; i < samples; ++i)
        peak = juce::jmax(peak, std::abs(input[i]));
    inputPeak.store(peak);

    const float gain = bypassEnabled.load()
        ? 1.0f
        : inputGainLinear.load() * micGainLinear.load() * outputGainLinear.load();

    // Preserve channel 0 and duplicate mono input to the remaining outputs.
    auto* left = buffer.getWritePointer(0, start);
    for (int i = 0; i < samples; ++i)
        left[i] = input[i] * gain;

    for (int channel = 1; channel < channels; ++channel)
    {
        auto* output = buffer.getWritePointer(channel, start);
        for (int i = 0; i < samples; ++i)
            output[i] = input[i] * gain;
    }
}

void MainComponent::releaseResources()
{
}

void MainComponent::timerCallback()
{
    inputMeter.setLevel(inputPeak.load());
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

    // Every parameter uses the same compact rotary-knob control.
    result->slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    result->slider.setTextBoxStyle(juce::Slider::TextBoxBelow,
        false, 68, 18);
    result->slider.setLookAndFeel(&result->rotaryLookAndFeel);
    result->slider.setRotaryParameters(
        juce::MathConstants<float>::pi * 1.2f,
        juce::MathConstants<float>::pi * 2.8f, true);

    result->slider.setRange(minimum, maximum, 0.1);
    result->slider.setValue(initial, juce::dontSendNotification);\n    result->slider.onValueChange = [this, name, slider = &result->slider]\n    {\n        const float value = (float)slider->getValue();\n        if (name == "Input Gain")\n            inputGainLinear.store(juce::Decibels::decibelsToGain(value));\n        else if (name == "Mic Volume")\n            micGainLinear.store(value / 100.0f);\n        else if (name == "Output Volume")\n            outputGainLinear.store(value / 100.0f);\n    };
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
    auto area = getLocalBounds().reduced(12);

    // Compact header: title on the left, input meter on the right.
    titleLabel.setBounds(area.getX(), area.getY() + 5, 340, 30);
    inputMeterLabel.setBounds(area.getRight() - 164, area.getY() + 1, 164, 13);
    inputMeter.setBounds(area.getRight() - 164, area.getY() + 15, 164, 43);

    area.removeFromTop(64);
    area.removeFromBottom(43);

    const int gap = 9;
    const int columnWidth = (area.getWidth() - gap) / 2;
    const int groupHeight = (area.getHeight() - gap) / 2;
    juce::ToggleButton* toggles[] = {
        &mixerEnabled, &eqEnabled, &compressorEnabled, &cleanupEnabled
    };

    // Avoid indexing parameter vectors until construction is complete.
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
            card.getX() + 11, card.getY() + 6, card.getWidth() - 88, 23);
        toggles[group]->setBounds(card.getRight() - 68, card.getY() + 6, 58, 22);

        if (!parametersReady)
            continue;

        const auto& controls = groups[(size_t)group];
        const int count = (int)controls.size();
        const int innerWidth = card.getWidth() - 18;
        const int slotWidth = innerWidth / 3;
        const int knobWidth = juce::jmin(82, slotWidth - 3);
        const int knobHeight = juce::jmin(94, card.getHeight() - 43);
        const int controlTop = card.getY() + 30;

        // Two-control groups are centered; three-control groups fill the row.
        const int usedWidth = count * slotWidth;
        const int firstSlotX = card.getX() + (card.getWidth() - usedWidth) / 2;

        for (int i = 0; i < count; ++i)
        {
            auto& control = *controls[(size_t)i];
            const int slotX = firstSlotX + i * slotWidth;
            const int knobX = slotX + (slotWidth - knobWidth) / 2;

            control.label.setJustificationType(juce::Justification::centred);
            control.label.setFont(juce::FontOptions(9.5f, juce::Font::bold));
            control.label.setBounds(slotX, controlTop, slotWidth, 17);
            control.slider.setBounds(knobX, controlTop + 15,
                                     knobWidth, knobHeight);
        }
    }

    const int footerY = getHeight() - 37;
    processButton.setBounds(12, footerY, 112, 27);
    bypassButton.setBounds(130, footerY, 82, 27);
    statusLabel.setBounds(222, footerY + 3, getWidth() - 234, 21);
}
