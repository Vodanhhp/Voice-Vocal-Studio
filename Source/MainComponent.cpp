#include "MainComponent.h"
#include <cmath>

namespace
{
    const juce::Colour bgTop { 23, 30, 42 };
    const juce::Colour bgBottom { 7, 10, 16 };
    const juce::Colour panelTop { 31, 42, 57 };
    const juce::Colour panelBottom { 13, 19, 28 };
    const juce::Colour edge { 74, 94, 116 };
    const juce::Colour bright { 235, 243, 252 };
    const juce::Colour muted { 153, 169, 187 };
    const juce::Colour blue { 28, 164, 255 };
    const juce::Colour cyan { 50, 213, 255 };
    const juce::Colour green { 46, 220, 113 };

    void drawPanel(juce::Graphics& g, juce::Rectangle<int> area,
                   const juce::String& title, juce::Colour iconColour = cyan)
    {
        auto r = area.toFloat();
        g.setColour(juce::Colours::black.withAlpha(0.48f));
        g.fillRoundedRectangle(r.translated(0.0f, 4.0f), 12.0f);
        g.setGradientFill(juce::ColourGradient(panelTop, r.getX(), r.getY(),
                                                panelBottom, r.getX(), r.getBottom(), false));
        g.fillRoundedRectangle(r, 12.0f);
        g.setColour(edge.withAlpha(0.85f));
        g.drawRoundedRectangle(r, 12.0f, 1.2f);
        g.setColour(juce::Colour(7, 12, 19).withAlpha(0.82f));
        g.fillRoundedRectangle(r.getX() + 1.5f, r.getY() + 1.5f,
                               r.getWidth() - 3.0f, 43.0f, 10.0f);
        g.setColour(iconColour);
        g.fillRoundedRectangle(r.getX() + 12.0f, r.getY() + 11.0f, 3.0f, 21.0f, 1.5f);
        g.setColour(bright);
        g.setFont(juce::FontOptions(16.0f, juce::Font::bold));
        g.drawText(title, area.getX() + 24, area.getY() + 7,
                   area.getWidth() - 115, 30, juce::Justification::centredLeft);
        g.setColour(green.withAlpha(0.22f));
        g.fillEllipse(r.getRight() - 29.0f, r.getY() + 14.0f, 12.0f, 12.0f);
        g.setColour(green);
        g.fillEllipse(r.getRight() - 25.5f, r.getY() + 17.5f, 5.0f, 5.0f);
    }
}

void StudioKnobLookAndFeel::drawRotarySlider(
    juce::Graphics& g, int x, int y, int width, int height,
    float sliderPos, float startAngle, float endAngle, juce::Slider& slider)
{
    juce::ignoreUnused(slider);
    const float diameter = (float)juce::jmin(width, height) - 13.0f;
    const float radius = diameter * 0.5f;
    const float cx = (float)x + (float)width * 0.5f;
    const float cy = (float)y + radius + 4.0f;
    const float angle = startAngle + sliderPos * (endAngle - startAngle);

    g.setColour(juce::Colour(3, 8, 14).withAlpha(0.8f));
    g.fillEllipse(cx - radius - 3.0f, cy - radius - 3.0f,
                  diameter + 6.0f, diameter + 6.0f);
    g.setColour(juce::Colour(91, 117, 145));
    g.drawEllipse(cx - radius, cy - radius, diameter, diameter, 1.6f);

    juce::Path ring;
    ring.addCentredArc(cx, cy, radius - 4.0f, radius - 4.0f, 0.0f,
                       startAngle, endAngle, true);
    g.setColour(juce::Colour(55, 77, 101));
    g.strokePath(ring, juce::PathStrokeType(3.5f));
    juce::Path active;
    active.addCentredArc(cx, cy, radius - 4.0f, radius - 4.0f, 0.0f,
                         startAngle, angle, true);
    g.setColour(blue);
    g.strokePath(active, juce::PathStrokeType(4.2f,
        juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

    juce::ColourGradient metal(juce::Colour(221, 228, 235), cx, cy - radius,
                               juce::Colour(66, 79, 94), cx, cy + radius, false);
    g.setGradientFill(metal);
    g.fillEllipse(cx - radius * 0.72f, cy - radius * 0.72f,
                  radius * 1.44f, radius * 1.44f);
    g.setColour(juce::Colour(15, 23, 33));
    g.drawEllipse(cx - radius * 0.72f, cy - radius * 0.72f,
                  radius * 1.44f, radius * 1.44f, 2.0f);

    g.setColour(juce::Colour(23, 34, 47));
    g.drawLine(cx, cy, cx + std::cos(angle) * radius * 0.55f,
               cy + std::sin(angle) * radius * 0.55f, 3.0f);
    g.setColour(juce::Colours::white);
    g.fillEllipse(cx - 3.0f, cy - 3.0f, 6.0f, 6.0f);
}

MainComponent::MainComponent()
{

    titleLabel.setText("Voice Vocal Studio", juce::dontSendNotification);
    titleLabel.setFont(juce::FontOptions(38.0f, juce::Font::bold | juce::Font::italic));
    titleLabel.setColour(juce::Label::textColourId, bright);
    addAndMakeVisible(titleLabel);

    subtitleLabel.setText("Hat hay hon - Truyen cam hon", juce::dontSendNotification);
    subtitleLabel.setFont(juce::FontOptions(14.0f, juce::Font::italic));
    subtitleLabel.setColour(juce::Label::textColourId, muted);
    addAndMakeVisible(subtitleLabel);

    deviceLabel.setText("THIET BI AM THANH", juce::dontSendNotification);
    deviceLabel.setFont(juce::FontOptions(10.0f, juce::Font::bold));
    deviceLabel.setColour(juce::Label::textColourId, muted);
    addAndMakeVisible(deviceLabel);

    deviceSelector.addItem("Windows Default Audio", 1);
    deviceSelector.setSelectedId(1);
    deviceSelector.setEnabled(false); // Device selection will be connected in the audio-device stage.
    deviceSelector.setColour(juce::ComboBox::backgroundColourId, juce::Colour(12, 18, 27));
    deviceSelector.setColour(juce::ComboBox::textColourId, bright);
    deviceSelector.setColour(juce::ComboBox::outlineColourId, edge);
    addAndMakeVisible(deviceSelector);

    connectionLabel.setText("●  AUDIO READY   |   DEVICE DEFAULT", juce::dontSendNotification);
    connectionLabel.setFont(juce::FontOptions(12.0f, juce::Font::bold));
    connectionLabel.setColour(juce::Label::textColourId, green);
    addAndMakeVisible(connectionLabel);

    styleToggle(monitorToggle);
    monitorToggle.setToggleState(true, juce::dontSendNotification);
    addAndMakeVisible(monitorToggle);

    for (auto& toggle : groupToggles)
    {
        styleToggle(toggle);
        toggle.setToggleState(true, juce::dontSendNotification);
        addAndMakeVisible(toggle);
    }

    // Left: input and output gain.
    addParameter("Input Gain", -12.0, 24.0, 0.0, " dB", 0);
    addParameter("Output Gain", -24.0, 12.0, 0.0, " dB", 0);

    // Parametric EQ: five visual bands. DSP connection is a later stage.
    addParameter("LOW  |  80 Hz", -12.0, 12.0, 3.0, " dB", 1);
    addParameter("LOW MID  |  250 Hz", -12.0, 12.0, 1.5, " dB", 1);
    addParameter("MID  |  1.0 kHz", -12.0, 12.0, 0.0, " dB", 1);
    addParameter("HIGH MID  |  4.0 kHz", -12.0, 12.0, 2.0, " dB", 1);
    addParameter("HIGH  |  12 kHz", -12.0, 12.0, 1.0, " dB", 1);

    addParameter("Threshold", -60.0, 0.0, -18.0, " dB", 2);
    addParameter("Ratio", 1.0, 12.0, 3.0, " : 1", 2);
    addParameter("Attack", 1.0, 100.0, 10.0, " ms", 2);
    addParameter("Release", 10.0, 500.0, 150.0, " ms", 2);

    addParameter("Threshold", -60.0, 0.0, -30.0, " dB", 3);
    addParameter("Frequency", 2000.0, 12000.0, 6000.0, " Hz", 3);

    addParameter("Presence", 0.0, 100.0, 60.0, " %", 4);
    addParameter("Warmth", 0.0, 100.0, 50.0, " %", 4);
    addParameter("Air", 0.0, 100.0, 70.0, " %", 4);

    addParameter("Threshold", -80.0, 0.0, -50.0, " dB", 5);
    addParameter("Hold", 0.0, 500.0, 200.0, " ms", 5);
    addParameter("Release", 0.0, 1000.0, 300.0, " ms", 5);

    addParameter("Bass", -12.0, 12.0, 2.0, " dB", 6);
    addParameter("Mid", -12.0, 12.0, 0.0, " dB", 6);
    addParameter("Treble", -12.0, 12.0, 2.0, " dB", 6);

    addParameter("Mix", 0.0, 100.0, 20.0, " %", 7);
    addParameter("Room", 0.0, 100.0, 50.0, " %", 7);
    addParameter("Time", 0.1, 5.0, 1.8, " s", 7);

    masterVolumeLabel.setText("MASTER VOLUME", juce::dontSendNotification);
    masterVolumeLabel.setFont(juce::FontOptions(13.0f, juce::Font::bold));
    masterVolumeLabel.setColour(juce::Label::textColourId, bright);
    addAndMakeVisible(masterVolumeLabel);

    masterValueLabel.setText("0.0 dB", juce::dontSendNotification);
    masterValueLabel.setFont(juce::FontOptions(13.0f, juce::Font::bold));
    masterValueLabel.setColour(juce::Label::textColourId, bright);
    masterValueLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(masterValueLabel);

    masterVolumeSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    masterVolumeSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    masterVolumeSlider.setRange(-24.0, 12.0, 0.1);
    masterVolumeSlider.setValue(0.0);
    masterVolumeSlider.setColour(juce::Slider::trackColourId, blue);
    masterVolumeSlider.setColour(juce::Slider::backgroundColourId, juce::Colour(8, 14, 22));
    masterVolumeSlider.setColour(juce::Slider::thumbColourId, cyan);
    masterVolumeSlider.onValueChange = [this]
    {
        const auto db = (float)masterVolumeSlider.getValue();
        outputGainLinear.store(juce::Decibels::decibelsToGain(db));
        masterValueLabel.setText(juce::String(db, 1) + " dB", juce::dontSendNotification);
    };
    addAndMakeVisible(masterVolumeSlider);

    presetSelector.addItem("Giong Nam Am Ap", 1);
    presetSelector.addItem("Giong Nam Day", 2);
    presetSelector.addItem("Karaoke Nhac Nhe", 3);
    presetSelector.setSelectedId(1);
    presetSelector.setColour(juce::ComboBox::backgroundColourId, juce::Colour(11, 17, 25));
    presetSelector.setColour(juce::ComboBox::textColourId, bright);
    presetSelector.setColour(juce::ComboBox::outlineColourId, edge);
    addAndMakeVisible(presetSelector);

    auto styleTextButton = [](juce::TextButton& button, juce::Colour colour)
    {
        button.setColour(juce::TextButton::buttonColourId, colour);
        button.setColour(juce::TextButton::buttonOnColourId, colour.brighter(0.15f));
        button.setColour(juce::TextButton::textColourOffId, bright);
        button.setColour(juce::TextButton::textColourOnId, bright);
    };
    styleTextButton(savePresetButton, juce::Colour(39, 55, 73));
    styleTextButton(managePresetButton, juce::Colour(39, 55, 73));
    styleTextButton(processButton, juce::Colour(15, 108, 202));
    styleTextButton(bypassButton, juce::Colour(25, 71, 113));
    styleTextButton(resetButton, juce::Colour(49, 58, 71));
    styleTextButton(settingsButton, juce::Colour(49, 58, 71));
    addAndMakeVisible(savePresetButton);
    addAndMakeVisible(managePresetButton);
    addAndMakeVisible(processButton);
    addAndMakeVisible(bypassButton);
    addAndMakeVisible(resetButton);
    addAndMakeVisible(settingsButton);

    statusLabel.setText("San sang - cac bo xu ly dang o che do giao dien", juce::dontSendNotification);
    statusLabel.setFont(juce::FontOptions(10.5f));
    statusLabel.setColour(juce::Label::textColourId, muted);
    addAndMakeVisible(statusLabel);

    processButton.onClick = [this]
    {
        processingEnabled.store(!processingEnabled.load());
        statusLabel.setText(processingEnabled.load()
            ? "Monitor dang chay - cac module DSP se duoc noi o buoc tiep theo"
            : "Monitor da tat", juce::dontSendNotification);
    };
    bypassButton.onClick = [this]
    {
        bypassEnabled.store(!bypassEnabled.load());
        statusLabel.setText(bypassEnabled.load() ? "BYPASS - tin hieu truc tiep"
                                                 : "Da tat Bypass", juce::dontSendNotification);
    };
    resetButton.onClick = [this]
    {
        for (auto& group : groups)
            for (auto& control : group)
                control->slider.setValue(control->slider.getDoubleClickReturnValue(),
                                         juce::sendNotification);
        masterVolumeSlider.setValue(0.0, juce::sendNotification);
        statusLabel.setText("Da dua cac gia tri ve mac dinh", juce::dontSendNotification);
    };
    savePresetButton.onClick = [this]
    {
        statusLabel.setText("Luu Preset se duoc bo sung sau", juce::dontSendNotification);
    };
    managePresetButton.onClick = [this]
    {
        statusLabel.setText("Quan ly Preset se duoc bo sung sau", juce::dontSendNotification);
    };
    settingsButton.onClick = [this]
    {
        statusLabel.setText("Cai dat thiet bi se duoc bo sung sau", juce::dontSendNotification);
    };

    setSize(1536, 1024);
    resized();
    setAudioChannels(1, 2);
    startTimerHz(30);
}

MainComponent::~MainComponent()
{
    stopTimer();
    shutdownAudio();
}

void MainComponent::styleToggle(juce::ToggleButton& toggle)
{
    toggle.setColour(juce::ToggleButton::textColourId, bright);
    toggle.setColour(juce::ToggleButton::tickColourId, blue);
    toggle.setColour(juce::ToggleButton::tickDisabledColourId, muted);
}

MainComponent::ParameterControl* MainComponent::addParameter(
    const juce::String& name, double minimum, double maximum, double initial,
    const juce::String& suffix, int group)
{
    if (group < 0 || group >= (int)groups.size())
        return nullptr;

    auto control = std::make_unique<ParameterControl>();
    auto* result = control.get();

    result->label.setText(name, juce::dontSendNotification);
    result->label.setFont(juce::FontOptions(11.0f, juce::Font::bold));
    result->label.setColour(juce::Label::textColourId, bright);
    result->label.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(result->label);

    result->slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    result->slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 76, 20);
    result->slider.setLookAndFeel(&result->lookAndFeel);
    result->slider.setRotaryParameters(juce::MathConstants<float>::pi * 1.2f,
                                       juce::MathConstants<float>::pi * 2.8f, true);
    result->slider.setRange(minimum, maximum, 0.1);
    result->slider.setValue(initial, juce::dontSendNotification);
    result->slider.setDoubleClickReturnValue(true, initial);
    result->slider.setTextValueSuffix(suffix);
    result->slider.setColour(juce::Slider::textBoxTextColourId, bright);
    result->slider.setColour(juce::Slider::textBoxBackgroundColourId, juce::Colour(9, 15, 23));
    result->slider.setColour(juce::Slider::textBoxOutlineColourId, edge);
    addAndMakeVisible(result->slider);

    result->slider.onValueChange = [this, name, slider = &result->slider]
    {
        const float value = (float)slider->getValue();
        if (name == "Input Gain")
            inputGainLinear.store(juce::Decibels::decibelsToGain(value));
        else if (name == "Output Gain")
            outputGainLinear.store(juce::Decibels::decibelsToGain(value));
    };

    groups[(size_t)group].push_back(std::move(control));
    return result;
}

MainComponent::PanelBounds MainComponent::calculatePanelBounds() const
{
    PanelBounds p;
    const int w = getWidth();
    const int h = getHeight();
    const int margin = 12;
    const int gap = 12;
    const int headerBottom = 122;
    const int footerHeight = 104;
    const int bottomHeight = juce::jmax(170, (h - headerBottom - footerHeight) / 4);
    const int bottomY = h - footerHeight - bottomHeight - margin;
    const int topHeight = bottomY - headerBottom - gap;
    const int leftWidth = juce::jlimit(250, 300, w / 5);
    const int rightWidth = juce::jlimit(330, 390, w / 4);
    const int centreWidth = w - margin * 2 - gap * 2 - leftWidth - rightWidth;
    const int rightX = margin + leftWidth + gap + centreWidth + gap;

    p[0] = { margin, headerBottom, leftWidth, topHeight };
    p[1] = { margin + leftWidth + gap, headerBottom, centreWidth, topHeight };
    const int rightTopHeight = (topHeight - gap) * 2 / 3;
    p[2] = { rightX, headerBottom, rightWidth, rightTopHeight };
    p[3] = { rightX, headerBottom + rightTopHeight + gap, rightWidth,
             topHeight - rightTopHeight - gap };

    const int bottomWidth = (w - margin * 2 - gap * 3) / 4;
    for (int i = 0; i < 4; ++i)
        p[(size_t)(i + 4)] = { margin + i * (bottomWidth + gap), bottomY,
                               bottomWidth, bottomHeight };

    return p;
}

void MainComponent::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();
    g.setGradientFill(juce::ColourGradient(bgTop, 0.0f, 0.0f,
                                            bgBottom, 0.0f, bounds.getBottom(), false));
    g.fillAll();

    // Header fascia.
    auto header = juce::Rectangle<float>(0.0f, 0.0f, (float)getWidth(), 112.0f);
    g.setGradientFill(juce::ColourGradient(juce::Colour(34, 45, 60), 0.0f, 0.0f,
                                            juce::Colour(9, 14, 22), 0.0f, 112.0f, false));
    g.fillRoundedRectangle(header, 14.0f);
    g.setColour(edge);
    g.drawRoundedRectangle(header.reduced(1.0f), 14.0f, 1.2f);

    // Neon microphone emblem.
    g.setColour(cyan.withAlpha(0.18f));
    g.fillEllipse(20.0f, 15.0f, 82.0f, 82.0f);
    g.setColour(cyan);
    g.drawRoundedRectangle(48.0f, 27.0f, 25.0f, 43.0f, 12.0f, 2.8f);
    juce::Path microphoneArc;
    microphoneArc.addArc(39.0f, 39.0f, 43.0f, 42.0f, 0.0f,
                         juce::MathConstants<float>::pi, true);
    g.strokePath(microphoneArc, juce::PathStrokeType(2.8f));
    g.drawLine(60.5f, 81.0f, 60.5f, 89.0f, 2.5f);
    g.drawLine(48.0f, 90.0f, 73.0f, 90.0f, 2.5f);

    // Decorative waveform between the title and device strip.
    juce::Path wave;
    wave.startNewSubPath(615.0f, 56.0f);
    const float waveValues[] { 0, -5, 8, -14, 20, -30, 14, -9, 5, 0 };
    for (int i = 0; i < 10; ++i)
        wave.lineTo(615.0f + i * 10.0f, 56.0f + waveValues[i]);
    g.setColour(cyan);
    g.strokePath(wave, juce::PathStrokeType(2.0f));

    const auto panels = calculatePanelBounds();
    drawPanel(g, panels[0], "Input / Output");
    drawPanel(g, panels[1], "Parametric Equalizer");
    drawPanel(g, panels[2], "Compressor");
    drawPanel(g, panels[3], "De-Esser");
    drawPanel(g, panels[4], "Voice Enhance", juce::Colour(255, 190, 65));
    drawPanel(g, panels[5], "Noise Gate");
    drawPanel(g, panels[6], "Tone Control");
    drawPanel(g, panels[7], "Reverb", juce::Colour(90, 190, 255));

    // Input/output LED meters styled like the reference studio interface.
    auto drawLedMeter = [&g](juce::Rectangle<float> area, float level, bool gainReduction)
    {
        g.setColour(juce::Colour(3, 7, 11));
        g.fillRoundedRectangle(area, 4.0f);
        g.setColour(edge);
        g.drawRoundedRectangle(area, 4.0f, 1.0f);
        const int segments = 20;
        const float gap = 2.0f;
        const float segmentH = (area.getHeight() - gap * (segments + 1)) / (float)segments;
        const int lit = juce::jlimit(0, segments,
            (int)std::round(level * (float)segments));
        for (int i = 0; i < segments; ++i)
        {
            const float y = area.getBottom() - gap - (float)(i + 1) * (segmentH + gap);
            juce::Colour c = i >= 17 ? juce::Colour(245, 65, 55)
                : i >= 13 ? juce::Colour(255, 177, 45)
                : juce::Colour(53, 220, 112);
            if (gainReduction)
                c = i >= 15 ? juce::Colour(255, 177, 45) : juce::Colour(48, 205, 111);
            g.setColour(i < lit ? c : c.withAlpha(0.13f));
            g.fillRoundedRectangle(area.getX() + 4.0f, y, area.getWidth() - 8.0f,
                                   segmentH, 1.5f);
        }
    };

    // Static visual meter guides; live signal levels will drive these in the meter stage.
    auto inputPanel = panels[0];
    drawLedMeter({ (float)inputPanel.getRight() - 48.0f,
                   (float)inputPanel.getY() + 72.0f, 28.0f,
                   (float)inputPanel.getHeight() * 0.34f }, 0.72f, false);
    drawLedMeter({ (float)inputPanel.getRight() - 48.0f,
                   (float)inputPanel.getY() + inputPanel.getHeight() * 0.53f,
                   28.0f, (float)inputPanel.getHeight() * 0.34f }, 0.55f, false);

    // Compressor gain-reduction meter.
    drawLedMeter({ (float)panels[2].getRight() - 34.0f,
                   (float)panels[2].getY() + 72.0f, 22.0f,
                   (float)panels[2].getHeight() - 96.0f }, 0.46f, true);

    // EQ response graph, with five editable-band markers as a visual guide.
    auto eq = panels[1].reduced(14);
    auto graph = juce::Rectangle<float>((float)eq.getX() + 8.0f,
        (float)eq.getY() + 54.0f, (float)eq.getWidth() - 16.0f,
        (float)eq.getHeight() * 0.43f);
    g.setColour(juce::Colour(7, 13, 21));
    g.fillRoundedRectangle(graph, 7.0f);
    g.setColour(juce::Colour(53, 73, 94));
    g.drawRoundedRectangle(graph, 7.0f, 1.0f);
    for (int i = 1; i < 5; ++i)
    {
        const float x = graph.getX() + graph.getWidth() * (float)i / 5.0f;
        g.drawVerticalLine((int)x, graph.getY(), graph.getBottom());
    }
    for (int i = 1; i < 4; ++i)
    {
        const float y = graph.getY() + graph.getHeight() * (float)i / 4.0f;
        g.drawHorizontalLine((int)y, graph.getX(), graph.getRight());
    }
    g.setColour(juce::Colour(90, 125, 160));
    g.drawHorizontalLine((int)graph.getCentreY(), graph.getX(), graph.getRight());

    // Blue spectrum silhouette behind the EQ response curve.
    juce::Path spectrum;
    spectrum.startNewSubPath(graph.getX(), graph.getBottom() - 2.0f);
    const int spectrumBars = 115;
    for (int i = 0; i < spectrumBars; ++i)
    {
        const float t = (float)i / (float)(spectrumBars - 1);
        const float envelope = 0.22f + 0.78f * std::exp(-2.7f * t);
        const float modulation = 0.22f + 0.78f * std::abs(
            std::sin((float)i * 1.73f) * std::cos((float)i * 0.37f));
        const float barHeight = graph.getHeight() * 0.48f * envelope * modulation;
        const float x = graph.getX() + t * graph.getWidth();
        spectrum.lineTo(x, graph.getBottom() - barHeight);
    }
    spectrum.lineTo(graph.getRight(), graph.getBottom() - 2.0f);
    spectrum.closeSubPath();
    g.setColour(juce::Colour(16, 111, 211).withAlpha(0.23f));
    g.fillPath(spectrum);

    juce::Path curve;
    curve.startNewSubPath(graph.getX(), graph.getCentreY() + 12.0f);
    curve.cubicTo(graph.getX() + graph.getWidth() * 0.10f, graph.getCentreY() + 10.0f,
                  graph.getX() + graph.getWidth() * 0.12f, graph.getCentreY() - 42.0f,
                  graph.getX() + graph.getWidth() * 0.22f, graph.getCentreY() - 36.0f);
    curve.cubicTo(graph.getX() + graph.getWidth() * 0.34f, graph.getCentreY() - 25.0f,
                  graph.getX() + graph.getWidth() * 0.36f, graph.getCentreY() + 9.0f,
                  graph.getX() + graph.getWidth() * 0.48f, graph.getCentreY() + 4.0f);
    curve.cubicTo(graph.getX() + graph.getWidth() * 0.62f, graph.getCentreY() - 1.0f,
                  graph.getX() + graph.getWidth() * 0.69f, graph.getCentreY() - 38.0f,
                  graph.getX() + graph.getWidth() * 0.78f, graph.getCentreY() - 28.0f);
    curve.cubicTo(graph.getX() + graph.getWidth() * 0.88f, graph.getCentreY() - 18.0f,
                  graph.getX() + graph.getWidth() * 0.91f, graph.getCentreY() + 7.0f,
                  graph.getRight(), graph.getCentreY() + 3.0f);
    g.setColour(blue);
    g.strokePath(curve, juce::PathStrokeType(2.2f));
    const float markerX[] { 0.22f, 0.36f, 0.52f, 0.78f, 0.91f };
    const float markerY[] { -36.0f, -18.0f, 2.0f, -28.0f, -12.0f };
    const juce::Colour markerColours[] { cyan, juce::Colour(255, 166, 55),
        green, juce::Colour(190, 110, 255), cyan };
    for (int i = 0; i < 5; ++i)
    {
        const float mx = graph.getX() + graph.getWidth() * markerX[i];
        const float my = graph.getCentreY() + markerY[i];
        g.setColour(markerColours[i].withAlpha(0.22f));
        g.fillEllipse(mx - 11.0f, my - 11.0f, 22.0f, 22.0f);
        g.setColour(markerColours[i]);
        g.fillEllipse(mx - 6.0f, my - 6.0f, 12.0f, 12.0f);
    }

    // Frequency scale below the response graph.
    g.setFont(juce::FontOptions(10.0f));
    g.setColour(muted);
    const juce::String freqLabels[] { "20", "50", "100", "200", "500", "1k", "2k", "5k", "10k", "20k" };
    for (int i = 0; i < 10; ++i)
    {
        const float x = graph.getX() + graph.getWidth() * (float)i / 9.0f;
        g.drawText(freqLabels[i], (int)x - 18, (int)graph.getBottom() + 3, 36, 15,
                   juce::Justification::centred);
    }

    // Footer plate.
    auto footer = juce::Rectangle<float>(0.0f, (float)getHeight() - 100.0f,
                                          (float)getWidth(), 100.0f);
    g.setGradientFill(juce::ColourGradient(juce::Colour(22, 30, 42), 0.0f, footer.getY(),
                                            juce::Colour(6, 10, 16), 0.0f, footer.getBottom(), false));
    g.fillRect(footer);
    g.setColour(edge);
    g.drawHorizontalLine((int)footer.getY(), 10.0f, (float)getWidth() - 10.0f);
}

void MainComponent::resized()
{
    titleLabel.setBounds(116, 18, 485, 52);
    subtitleLabel.setBounds(174, 67, 340, 22);
    deviceLabel.setBounds(810, 17, 255, 16);
    deviceSelector.setBounds(810, 36, 300, 45);
    connectionLabel.setBounds(1140, 37, 330, 42);

    const auto panels = calculatePanelBounds();
    for (int i = 0; i < 8; ++i)
    {
        auto area = panels[(size_t)i].reduced(12);
        groupToggles[(size_t)i].setBounds(area.getRight() - 74, area.getY() + 4, 66, 28);
    }

    // Input and output controls are large, stacked rotary controls.
    {
        auto a = panels[0].reduced(10);
        const int knobW = juce::jmin(145, a.getWidth() - 82);
        const int knobH = juce::jmin(174, (a.getHeight() - 105) / 2);
        const int x = a.getX() + 5;
        const int inputControlCount = juce::jmin(2, (int)groups[0].size());
        for (int i = 0; i < inputControlCount; ++i)
        {
            auto& c = *groups[0][(size_t)i];
            const int y = a.getY() + 48 + i * (knobH + 18);
            c.label.setBounds(a.getX(), y, a.getWidth(), 22);
            c.slider.setBounds(x, y + 20, knobW, knobH);
        }
        monitorToggle.setBounds(a.getX() + 8, a.getBottom() - 34, a.getWidth() - 16, 28);
    }

    // EQ: five compact knobs in a single row under the response curve.
    {
        auto a = panels[1].reduced(12);
        const int top = a.getY() + (int)(a.getHeight() * 0.54f);
        const int slot = a.getWidth() / 5;
        const int knobW = juce::jmin(132, slot - 8);
        const int knobH = juce::jmin(164, a.getBottom() - top - 6);
        const int eqControlCount = juce::jmin(5, (int)groups[1].size());
        for (int i = 0; i < eqControlCount; ++i)
        {
            auto& c = *groups[1][(size_t)i];
            const int x = a.getX() + i * slot;
            c.label.setBounds(x, top, slot, 24);
            c.slider.setBounds(x + (slot - knobW) / 2, top + 22, knobW, knobH);
        }
    }

    auto layoutSmallGroup = [this](int group, juce::Rectangle<int> panel, int columns)
    {
        auto a = panel.reduced(9);
        const int count = (int)groups[(size_t)group].size();
        if (count <= 0 || columns <= 0)
            return;

        const int rows = (count + columns - 1) / columns;
        if (rows <= 0)
            return;
        const int reservedRight = group == 2 ? 36 : 0;
        a.setRight(a.getRight() - reservedRight);
        const int slotW = a.getWidth() / columns;
        const int slotH = (a.getHeight() - 45) / rows;
        const int knobW = juce::jmin(116, slotW - 4);
        const int knobH = juce::jmin(126, slotH - 20);
        for (int i = 0; i < count; ++i)
        {
            auto& c = *groups[(size_t)group][(size_t)i];
            const int col = i % columns;
            const int row = i / columns;
            const int x = a.getX() + col * slotW;
            const int y = a.getY() + 44 + row * slotH;
            c.label.setBounds(x, y, slotW, 20);
            c.slider.setBounds(x + (slotW - knobW) / 2, y + 18, knobW, knobH);
        }
    };

    layoutSmallGroup(2, panels[2], 2);
    layoutSmallGroup(3, panels[3], 2);
    for (int group = 4; group < 8; ++group)
        layoutSmallGroup(group, panels[(size_t)group], 3);

    const int fy = getHeight() - 88;
    // Three footer zones: preset management, transport controls and master output.
    presetSelector.setBounds(145, fy + 16, 205, 36);
    savePresetButton.setBounds(360, fy + 16, 125, 36);
    managePresetButton.setBounds(495, fy + 16, 125, 36);
    processButton.setBounds(735, fy + 16, 112, 36);
    bypassButton.setBounds(857, fy + 16, 105, 36);
    resetButton.setBounds(972, fy + 16, 95, 36);
    settingsButton.setBounds(1077, fy + 16, 105, 36);
    masterVolumeLabel.setBounds(1198, fy + 5, 180, 24);
    masterVolumeSlider.setBounds(1198, fy + 32, 245, 26);
    masterValueLabel.setBounds(1442, fy + 30, 78, 30);
    statusLabel.setBounds(22, fy + 62, 1120, 22);
}

void MainComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    juce::ignoreUnused(samplesPerBlockExpected, sampleRate);
}

void MainComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo& info)
{
    if (info.buffer == nullptr || info.numSamples <= 0)
        return;

    auto& buffer = *info.buffer;
    const int channels = buffer.getNumChannels();
    if (channels <= 0)
        return;

    if (!processingEnabled.load() && !bypassEnabled.load())
    {
        buffer.clear(info.startSample, info.numSamples);
        inputPeak.store(0.0f);
        return;
    }

    const float* input = buffer.getReadPointer(0, info.startSample);
    float peak = 0.0f;
    for (int i = 0; i < info.numSamples; ++i)
        peak = juce::jmax(peak, std::abs(input[i]));
    inputPeak.store(peak);

    const float gain = bypassEnabled.load()
        ? 1.0f
        : inputGainLinear.load() * outputGainLinear.load();

    auto* left = buffer.getWritePointer(0, info.startSample);
    for (int i = 0; i < info.numSamples; ++i)
        left[i] = input[i] * gain;

    for (int ch = 1; ch < channels; ++ch)
        buffer.copyFrom(ch, info.startSample, buffer, 0, info.startSample, info.numSamples);
}

void MainComponent::releaseResources()
{
}

void MainComponent::timerCallback()
{
    // The input meter will be drawn as a live LED bar in a later audio-meter pass.
    repaint();
}
