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

    using BiquadCoefficients = std::array<float, 5>;

    BiquadCoefficients normaliseBiquad(float b0, float b1, float b2,
                                       float a0, float a1, float a2)
    {
        if (std::abs(a0) < 1.0e-9f)
            return { 1.0f, 0.0f, 0.0f, 0.0f, 0.0f };

        return { b0 / a0, b1 / a0, b2 / a0, a1 / a0, a2 / a0 };
    }

    BiquadCoefficients makePeakingCoefficients(float sampleRate, float frequency,
                                                float gainDb, float q)
    {
        if (std::abs(gainDb) < 0.001f)
            return { 1.0f, 0.0f, 0.0f, 0.0f, 0.0f };

        const float safeRate = juce::jmax(8000.0f, sampleRate);
        const float f = juce::jlimit(20.0f, safeRate * 0.45f, frequency);
        const float A = std::pow(10.0f, gainDb / 40.0f);
        const float w0 = juce::MathConstants<float>::twoPi * f / safeRate;
        const float cosine = std::cos(w0);
        const float alpha = std::sin(w0) / (2.0f * q);

        return normaliseBiquad(1.0f + alpha * A, -2.0f * cosine,
                               1.0f - alpha * A,
                               1.0f + alpha / A, -2.0f * cosine,
                               1.0f - alpha / A);
    }

    BiquadCoefficients makeLowShelfCoefficients(float sampleRate, float frequency,
                                                 float gainDb)
    {
        if (std::abs(gainDb) < 0.001f)
            return { 1.0f, 0.0f, 0.0f, 0.0f, 0.0f };

        const float safeRate = juce::jmax(8000.0f, sampleRate);
        const float f = juce::jlimit(20.0f, safeRate * 0.45f, frequency);
        const float A = std::pow(10.0f, gainDb / 40.0f);
        const float w0 = juce::MathConstants<float>::twoPi * f / safeRate;
        const float cosine = std::cos(w0);
        const float sine = std::sin(w0);
        const float alpha = sine * 0.5f * std::sqrt(2.0f);
        const float beta = 2.0f * std::sqrt(A) * alpha;

        return normaliseBiquad(
            A * ((A + 1.0f) - (A - 1.0f) * cosine + beta),
            2.0f * A * ((A - 1.0f) - (A + 1.0f) * cosine),
            A * ((A + 1.0f) - (A - 1.0f) * cosine - beta),
            (A + 1.0f) + (A - 1.0f) * cosine + beta,
            -2.0f * ((A - 1.0f) + (A + 1.0f) * cosine),
            (A + 1.0f) + (A - 1.0f) * cosine - beta);
    }

    BiquadCoefficients makeHighShelfCoefficients(float sampleRate, float frequency,
                                                  float gainDb)
    {
        if (std::abs(gainDb) < 0.001f)
            return { 1.0f, 0.0f, 0.0f, 0.0f, 0.0f };

        const float safeRate = juce::jmax(8000.0f, sampleRate);
        const float f = juce::jlimit(20.0f, safeRate * 0.45f, frequency);
        const float A = std::pow(10.0f, gainDb / 40.0f);
        const float w0 = juce::MathConstants<float>::twoPi * f / safeRate;
        const float cosine = std::cos(w0);
        const float sine = std::sin(w0);
        const float alpha = sine * 0.5f * std::sqrt(2.0f);
        const float beta = 2.0f * std::sqrt(A) * alpha;

        return normaliseBiquad(
            A * ((A + 1.0f) + (A - 1.0f) * cosine + beta),
            -2.0f * A * ((A - 1.0f) + (A + 1.0f) * cosine),
            A * ((A + 1.0f) + (A - 1.0f) * cosine - beta),
            (A + 1.0f) - (A - 1.0f) * cosine + beta,
            2.0f * ((A - 1.0f) - (A + 1.0f) * cosine),
            (A + 1.0f) - (A - 1.0f) * cosine - beta);
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

    deviceSelector.addItem("Am thanh mac dinh cua Windows", 1);
    deviceSelector.setSelectedId(1);
    deviceSelector.setEnabled(false); // Device selection will be connected in the audio-device stage.
    deviceSelector.setColour(juce::ComboBox::backgroundColourId, juce::Colour(12, 18, 27));
    deviceSelector.setColour(juce::ComboBox::textColourId, bright);
    deviceSelector.setColour(juce::ComboBox::outlineColourId, edge);
    addAndMakeVisible(deviceSelector);

    connectionLabel.setText("*  AM THANH SAN SANG  |  MAC DINH", juce::dontSendNotification);
    connectionLabel.setFont(juce::FontOptions(12.0f, juce::Font::bold));
    connectionLabel.setColour(juce::Label::textColourId, green);
    addAndMakeVisible(connectionLabel);

    styleToggle(monitorToggle);
    monitorToggle.setToggleState(true, juce::dontSendNotification);
    addAndMakeVisible(monitorToggle);

    for (size_t i = 0; i < groupToggles.size(); ++i)
    {
        auto& toggle = groupToggles[i];
        styleToggle(toggle);
        toggle.setToggleState(true, juce::dontSendNotification);
        groupEnabled[i].store(true);
        toggle.onClick = [this, i]
        {
            const bool enabled = groupToggles[i].getToggleState();
            groupEnabled[i].store(enabled);
            groupToggles[i].setButtonText(enabled ? "BAT" : "TAT");
        };
        addAndMakeVisible(toggle);
    }

    // Left: input and output gain.
    addParameter("Do loi dau vao", -12.0, 24.0, 0.0, " dB", 0);
    addParameter("Do loi dau ra", -24.0, 12.0, 0.0, " dB", 0);

    // Parametric EQ: five visual bands. DSP connection is a later stage.
    addParameter("TRAM  |  80 Hz", -12.0, 12.0, 3.0, " dB", 1);
    addParameter("TRAM VUA  |  250 Hz", -12.0, 12.0, 1.5, " dB", 1);
    addParameter("TRUNG  |  1,0 kHz", -12.0, 12.0, 0.0, " dB", 1);
    addParameter("CAO VUA  |  4,0 kHz", -12.0, 12.0, 2.0, " dB", 1);
    addParameter("CAO  |  12 kHz", -12.0, 12.0, 1.0, " dB", 1);

    addParameter("Nguong", -60.0, 0.0, -18.0, " dB", 2);
    addParameter("Ty le nen", 1.0, 12.0, 3.0, " : 1", 2);
    addParameter("Thoi gian tan cong", 1.0, 100.0, 10.0, " ms", 2);
    addParameter("Thoi gian nha", 10.0, 500.0, 150.0, " ms", 2);

    addParameter("Nguong", -60.0, 0.0, -30.0, " dB", 3);
    addParameter("Tan so", 2000.0, 12000.0, 6000.0, " Hz", 3);

    addParameter("Do hien dien", 0.0, 100.0, 60.0, " %", 4);
    addParameter("Do am", 0.0, 100.0, 50.0, " %", 4);
    addParameter("Do thoang", 0.0, 100.0, 70.0, " %", 4);

    addParameter("Nguong", -80.0, 0.0, -50.0, " dB", 5);
    addParameter("Thoi gian giu", 0.0, 500.0, 200.0, " ms", 5);
    addParameter("Thoi gian nha", 0.0, 1000.0, 300.0, " ms", 5);

    addParameter("Am tram", -12.0, 12.0, 2.0, " dB", 6);
    addParameter("Am trung", -12.0, 12.0, 0.0, " dB", 6);
    addParameter("Am cao", -12.0, 12.0, 2.0, " dB", 6);

    addParameter("Ty le hoa tron", 0.0, 100.0, 20.0, " %", 7);
    addParameter("Khong gian phong", 0.0, 100.0, 50.0, " %", 7);
    addParameter("Thoi gian", 0.1, 5.0, 1.8, " s", 7);

    masterVolumeLabel.setText("AM LUONG TONG", juce::dontSendNotification);
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
        masterVolumeLinear.store(juce::Decibels::decibelsToGain(db));
        masterValueLabel.setText(juce::String(db, 1) + " dB", juce::dontSendNotification);
    };
    addAndMakeVisible(masterVolumeSlider);

    presetSelector.addItem("Giong nam am ap", 1);
    presetSelector.addItem("Giong nam day", 2);
    presetSelector.addItem("Karaoke nhac nhe", 3);
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

    monitorToggle.onClick = [this]
    {
        const bool enabled = monitorToggle.getToggleState();
        monitorEnabled.store(enabled);
        monitorToggle.setButtonText(enabled ? "THEO DOI AM THANH: BAT" : "THEO DOI AM THANH: TAT");
        statusLabel.setText(enabled ? "Da bat theo doi am thanh truc tiep"
                                    : "Da tat theo doi - khong phat am thanh ra loa",
                            juce::dontSendNotification);
    };

    processButton.onClick = [this]
    {
        processingEnabled.store(!processingEnabled.load());
        processButton.setButtonText(processingEnabled.load() ? "DUNG XU LY" : "BAT XU LY");
        statusLabel.setText(processingEnabled.load()
            ? "Da bat tang xu ly tin hieu; cac hieu ung DSP se duoc bo sung tiep theo"
            : "Da tat xu ly tin hieu", juce::dontSendNotification);
    };
    bypassButton.onClick = [this]
    {
        bypassEnabled.store(!bypassEnabled.load());
        statusLabel.setText(bypassEnabled.load() ? "BO QUA - tin hieu truc tiep"
                                                 : "Da tat che do bo qua", juce::dontSendNotification);
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
        statusLabel.setText("Chuc nang luu cai dat se duoc bo sung sau", juce::dontSendNotification);
    };
    managePresetButton.onClick = [this]
    {
        statusLabel.setText("Chuc nang quan ly cai dat se duoc bo sung sau", juce::dontSendNotification);
    };
    settingsButton.onClick = [this]
    {
        showAudioSettings();
    };

    setSize(1280, 800);
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
    const int parameterIndex = (int)groups[(size_t)group].size();

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

    if (group == 1 && parameterIndex >= 0 && parameterIndex < (int)eqGainDb.size())
        eqGainDb[(size_t)parameterIndex].store((float)initial);

    result->slider.onValueChange = [this, name, group, parameterIndex,
                                    slider = &result->slider]
    {
        const float value = (float)slider->getValue();
        if (name == "Do loi dau vao")
            inputGainLinear.store(juce::Decibels::decibelsToGain(value));
        else if (name == "Do loi dau ra")
            outputGainLinear.store(juce::Decibels::decibelsToGain(value));
        else if (group == 1 && parameterIndex >= 0
                 && parameterIndex < (int)eqGainDb.size())
            eqGainDb[(size_t)parameterIndex].store(value);
    };

    groups[(size_t)group].push_back(std::move(control));
    return result;
}

MainComponent::PanelBounds MainComponent::calculatePanelBounds() const
{
    PanelBounds p;
    const int w = getWidth();
    const int h = getHeight();
    const int margin = juce::jlimit(8, 14, w / 110);
    const int gap = juce::jlimit(8, 12, w / 140);
    const int headerBottom = juce::jlimit(88, 108, h / 7);
    const int footerHeight = juce::jlimit(76, 92, h / 9);
    const int availablePanelsHeight = h - headerBottom - footerHeight - margin - gap;
    const int bottomHeight = juce::jlimit(142, 205, (int)(availablePanelsHeight * 0.36f));
    const int bottomY = h - footerHeight - bottomHeight - margin;
    const int topHeight = juce::jmax(190, bottomY - headerBottom - gap);
    const int leftWidth = juce::jlimit(205, 270, (int)(w * 0.205f));
    const int rightWidth = juce::jlimit(270, 340, (int)(w * 0.255f));
    const int centreWidth = w - margin * 2 - gap * 2 - leftWidth - rightWidth;
    const int rightX = margin + leftWidth + gap + centreWidth + gap;

    p[0] = { margin, headerBottom, leftWidth, topHeight };
    p[1] = { margin + leftWidth + gap, headerBottom, centreWidth, topHeight };
    const int rightTopHeight = (topHeight - gap) * 58 / 100;
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
    const float headerHeight = (float)juce::jlimit(88, 108, getHeight() / 7);
    auto header = juce::Rectangle<float>(0.0f, 0.0f, (float)getWidth(), headerHeight);
    g.setGradientFill(juce::ColourGradient(juce::Colour(34, 45, 60), 0.0f, 0.0f,
                                            juce::Colour(9, 14, 22), 0.0f, headerHeight, false));
    g.fillRoundedRectangle(header, 14.0f);
    g.setColour(edge);
    g.drawRoundedRectangle(header.reduced(1.0f), 14.0f, 1.2f);

    // Neon microphone emblem.
    const float logoScale = juce::jlimit(0.72f, 1.0f, headerHeight / 108.0f);
    g.setColour(cyan.withAlpha(0.18f));
    g.fillEllipse(18.0f, 9.0f, 78.0f * logoScale, 78.0f * logoScale);
    g.setColour(cyan);
    g.drawRoundedRectangle(45.0f, 20.0f, 24.0f, 40.0f, 11.0f, 2.6f);
    juce::Path microphoneArc;
    microphoneArc.addArc(37.0f, 31.0f, 40.0f, 40.0f, 0.0f,
                         juce::MathConstants<float>::pi, true);
    g.strokePath(microphoneArc, juce::PathStrokeType(2.6f));
    g.drawLine(57.0f, 70.0f, 57.0f, 78.0f, 2.3f);
    g.drawLine(45.0f, 79.0f, 69.0f, 79.0f, 2.3f);

    // Decorative waveform between the title and device strip.
    juce::Path wave;
    wave.startNewSubPath((float)getWidth() * 0.43f, headerHeight * 0.50f);
    const float waveValues[] { 0, -5, 8, -14, 20, -30, 14, -9, 5, 0 };
    for (int i = 0; i < 10; ++i)
        wave.lineTo((float)getWidth() * 0.43f + i * 9.0f, headerHeight * 0.50f + waveValues[i] * 0.72f);
    g.setColour(cyan);
    g.strokePath(wave, juce::PathStrokeType(2.0f));

    const auto panels = calculatePanelBounds();
    drawPanel(g, panels[0], "DAU VAO / DAU RA");
    drawPanel(g, panels[1], "BO LOC EQ THAM SO");
    drawPanel(g, panels[2], "NEN AM THANH");
    drawPanel(g, panels[3], "GIAM AM XI");
    drawPanel(g, panels[4], "CAI THIEN GIONG HAT", juce::Colour(255, 190, 65));
    drawPanel(g, panels[5], "CONG CHAN TIENG ON");
    drawPanel(g, panels[6], "DIEU CHINH AM SAC");
    drawPanel(g, panels[7], "TAO VANG", juce::Colour(90, 190, 255));

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
    const float livePeak = juce::jlimit(0.0f, 1.0f, inputPeak.load());
    drawLedMeter({ (float)inputPanel.getRight() - 48.0f,
                   (float)inputPanel.getY() + 72.0f, 28.0f,
                   (float)inputPanel.getHeight() * 0.34f }, livePeak, false);
    drawLedMeter({ (float)inputPanel.getRight() - 48.0f,
                   (float)inputPanel.getY() + inputPanel.getHeight() * 0.53f,
                   28.0f, (float)inputPanel.getHeight() * 0.34f }, livePeak * 0.82f, false);

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

    // Draw the actual combined EQ response from the five current knob values.
    // The frequency axis is logarithmic, matching a conventional parametric EQ.
    const float bandGains[] {
        eqGainDb[0].load(), eqGainDb[1].load(), eqGainDb[2].load(),
        eqGainDb[3].load(), eqGainDb[4].load()
    };
    const float bandFrequencies[] { 80.0f, 250.0f, 1000.0f, 4000.0f, 12000.0f };
    const float logMin = std::log(20.0f);
    const float logMax = std::log(20000.0f);
    auto responseAt = [&bandGains, &bandFrequencies, logMin, logMax](float frequency)
    {
        const float safeFrequency = juce::jmax(20.0f, frequency);
        const float logFrequency = std::log(safeFrequency);
        const float lowShelf = bandGains[0]
            / (1.0f + std::pow(safeFrequency / bandFrequencies[0], 4.0f));
        const float highShelf = bandGains[4]
            / (1.0f + std::pow(bandFrequencies[4] / safeFrequency, 4.0f));
        float response = lowShelf + highShelf;
        for (int band = 1; band < 4; ++band)
        {
            const float distance = (logFrequency - std::log(bandFrequencies[band])) / 0.72f;
            response += bandGains[band] * std::exp(-0.5f * distance * distance);
        }
        juce::ignoreUnused(logMin, logMax);
        return juce::jlimit(-12.0f, 12.0f, response);
    };
    auto frequencyToX = [&graph, logMin, logMax](float frequency)
    {
        const float position = (std::log(juce::jlimit(20.0f, 20000.0f, frequency)) - logMin)
                             / (logMax - logMin);
        return graph.getX() + position * graph.getWidth();
    };
    auto responseToY = [&graph](float gainDb)
    {
        return graph.getCentreY() - (gainDb / 12.0f) * (graph.getHeight() * 0.44f);
    };

    juce::Path curve;
    constexpr int curvePoints = 180;
    for (int i = 0; i < curvePoints; ++i)
    {
        const float t = (float)i / (float)(curvePoints - 1);
        const float frequency = std::exp(logMin + t * (logMax - logMin));
        const float x = graph.getX() + t * graph.getWidth();
        const float y = responseToY(responseAt(frequency));
        if (i == 0)
            curve.startNewSubPath(x, y);
        else
            curve.lineTo(x, y);
    }

    juce::Path curveFill = curve;
    curveFill.lineTo(graph.getRight(), graph.getCentreY());
    curveFill.lineTo(graph.getX(), graph.getCentreY());
    curveFill.closeSubPath();
    g.setColour(cyan.withAlpha(0.10f));
    g.fillPath(curveFill);
    g.setColour(blue);
    g.strokePath(curve, juce::PathStrokeType(2.4f,
        juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

    const juce::Colour markerColours[] { cyan, juce::Colour(255, 166, 55),
        green, juce::Colour(190, 110, 255), cyan };
    for (int i = 0; i < 5; ++i)
    {
        const float mx = frequencyToX(bandFrequencies[i]);
        const float my = responseToY(responseAt(bandFrequencies[i]));
        g.setColour(markerColours[i].withAlpha(0.22f));
        g.fillEllipse(mx - 11.0f, my - 11.0f, 22.0f, 22.0f);
        g.setColour(markerColours[i]);
        g.fillEllipse(mx - 5.5f, my - 5.5f, 11.0f, 11.0f);
    }

    // Tan so scale below the response graph.
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
    const float footerHeight = (float)juce::jlimit(76, 92, getHeight() / 9);
    auto footer = juce::Rectangle<float>(0.0f, (float)getHeight() - footerHeight,
                                          (float)getWidth(), footerHeight);
    g.setGradientFill(juce::ColourGradient(juce::Colour(22, 30, 42), 0.0f, footer.getY(),
                                            juce::Colour(6, 10, 16), 0.0f, footer.getBottom(), false));
    g.fillRect(footer);
    g.setColour(edge);
    g.drawHorizontalLine((int)footer.getY(), 10.0f, (float)getWidth() - 10.0f);

    // Recessed footer zones mirror the three control groups in the approved mockup.
    const float fy = footer.getY() + 6.0f;
    const float w = (float)getWidth();
    const juce::Rectangle<float> footerZones[] {
        { 10.0f, fy, w * 0.34f, footerHeight - 12.0f },
        { w * 0.36f, fy, w * 0.29f, footerHeight - 12.0f },
        { w * 0.67f, fy, w * 0.32f, footerHeight - 12.0f }
    };
    for (const auto& zone : footerZones)
    {
        g.setColour(juce::Colour(3, 7, 12).withAlpha(0.78f));
        g.fillRoundedRectangle(zone, 12.0f);
        g.setColour(edge.withAlpha(0.8f));
        g.drawRoundedRectangle(zone, 12.0f, 1.1f);
    }
}

void MainComponent::resized()
{
    const int w = getWidth();
    const int h = getHeight();
    const int headerH = juce::jlimit(88, 108, h / 7);
    titleLabel.setBounds(108, 12, juce::jmin(420, (int)(w * 0.36f)), 46);
    subtitleLabel.setBounds(150, 56, juce::jmin(330, (int)(w * 0.28f)), 20);
    const int deviceX = (int)(w * 0.56f);
    deviceLabel.setBounds(deviceX, 10, (int)(w * 0.18f), 16);
    deviceSelector.setBounds(deviceX, 29, (int)(w * 0.20f), headerH - 38);
    connectionLabel.setBounds((int)(w * 0.79f), 28, (int)(w * 0.19f), 38);

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
        const int x = a.getX() + 5;
        const int inputControlCount = juce::jmin(2, (int)groups[0].size());
        const int firstRowY = a.getY() + 48;
        const int monitorTop = a.getBottom() - 38;
        const int rowAreaHeight = juce::jmax(120, monitorTop - firstRowY - 10);
        const int rowHeight = rowAreaHeight / juce::jmax(1, inputControlCount);
        const int labelHeight = 22;
        const int valueBoxAndGap = 26;
        const int sliderHeight = juce::jmax(70, rowHeight - labelHeight - valueBoxAndGap);
        for (int i = 0; i < inputControlCount; ++i)
        {
            auto& c = *groups[0][(size_t)i];
            const int rowY = firstRowY + i * rowHeight;
            c.label.setBounds(a.getX(), rowY, a.getWidth(), labelHeight);
            c.slider.setBounds(x, rowY + labelHeight, knobW, sliderHeight);
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

    layoutSmallGroup(2, panels[2], 4);
    layoutSmallGroup(3, panels[3], 2);
    for (int group = 4; group < 8; ++group)
        layoutSmallGroup(group, panels[(size_t)group], 3);

    const int footerTop = getHeight() - juce::jlimit(76, 92, getHeight() / 9);
    const int footerY = footerTop + 12;
    const int leftX = 18;
    presetSelector.setBounds(leftX, footerY, 150, 34);
    savePresetButton.setBounds(leftX + 158, footerY, 92, 34);
    managePresetButton.setBounds(leftX + 258, footerY, 102, 34);

    const int centreX = (int)(w * 0.36f);
    processButton.setBounds(centreX, footerY, 112, 34);
    bypassButton.setBounds(centreX + 120, footerY, 82, 34);
    resetButton.setBounds(centreX + 210, footerY, 74, 34);
    settingsButton.setBounds(centreX + 292, footerY, 86, 34);

    const int masterX = (int)(w * 0.69f);
    masterVolumeLabel.setBounds(masterX, footerTop + 5, 170, 20);
    masterVolumeSlider.setBounds(masterX, footerY + 3, juce::jmax(110, w - masterX - 105), 25);
    masterValueLabel.setBounds(w - 92, footerY, 82, 30);
    statusLabel.setBounds(18, footerTop + 54, w - 36, 18);
}

void MainComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    juce::ignoreUnused(samplesPerBlockExpected);
    currentSampleRate = sampleRate > 0.0 ? sampleRate : 44100.0;
    for (auto& filter : eqFilters)
        filter.reset();
    lastAppliedEqGainDb.fill(999.0f);
}

void MainComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo& info)
{
    if (info.buffer == nullptr || info.numSamples <= 0)
        return;

    auto& buffer = *info.buffer;
    const int channels = buffer.getNumChannels();
    if (channels <= 0)
        return;

    // Monitor is the master route switch. When disabled, do not send microphone
    // audio to the outputs. This is independent from the processing/bypass state.
    if (!monitorEnabled.load())
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

    // Process currently enables the input/output gain stage. Actual DSP modules
    // (EQ, compressor, etc.) will be inserted here in later steps.
    // Bypass skips that stage, while Master Volume remains active.
    const bool useGainStage = processingEnabled.load() && !bypassEnabled.load();
    const bool eqActive = useGainStage && groupEnabled[1].load();
    const float channelGain = useGainStage
        ? inputGainLinear.load() * outputGainLinear.load()
        : 1.0f;
    const float masterGain = masterVolumeLinear.load();

    if (eqActive)
    {
        const float sampleRate = (float)currentSampleRate;
        for (size_t band = 0; band < eqFilters.size(); ++band)
        {
            const float gainDb = eqGainDb[band].load();
            if (std::abs(gainDb - lastAppliedEqGainDb[band]) > 0.001f)
            {
                BiquadCoefficients coefficients;
                if (band == 0)
                    coefficients = makeLowShelfCoefficients(sampleRate, 80.0f, gainDb);
                else if (band == 4)
                    coefficients = makeHighShelfCoefficients(sampleRate, 12000.0f, gainDb);
                else
                {
                    const float frequencies[] { 0.0f, 250.0f, 1000.0f, 4000.0f, 0.0f };
                    coefficients = makePeakingCoefficients(sampleRate, frequencies[band],
                                                           gainDb, 0.8f);
                }

                eqFilters[band].setCoefficients(coefficients[0], coefficients[1],
                    coefficients[2], coefficients[3], coefficients[4]);
                lastAppliedEqGainDb[band] = gainDb;
            }
        }
    }
    else
    {
        for (auto& filter : eqFilters)
            filter.reset();
    }

    auto* left = buffer.getWritePointer(0, info.startSample);
    for (int i = 0; i < info.numSamples; ++i)
    {
        float sample = input[i] * channelGain;
        if (eqActive)
            for (auto& filter : eqFilters)
                sample = filter.process(sample);
        left[i] = sample * masterGain;
    }

    // Duplicate the mono microphone signal to every enabled output channel.
    for (int ch = 1; ch < channels; ++ch)
        buffer.copyFrom(ch, info.startSample, buffer, 0, info.startSample, info.numSamples);
}

void MainComponent::releaseResources()
{
}

namespace
{
    class AudioSettingsContent final : public juce::Component
    {
    public:
        explicit AudioSettingsContent(juce::AudioDeviceManager& manager)
            : selector(manager, 1, 2, 1, 2, false, false, true, true)
        {
            addAndMakeVisible(selector);
            setSize(760, 560);
        }

        void resized() override
        {
            selector.setBounds(getLocalBounds().reduced(14));
        }

    private:
        juce::AudioDeviceSelectorComponent selector;
    };
}

void MainComponent::showAudioSettings()
{
    juce::DialogWindow::LaunchOptions options;
    options.content.setOwned(new AudioSettingsContent(deviceManager));
    options.dialogTitle = "Cai dat thiet bi am thanh";
    options.dialogBackgroundColour = juce::Colour(18, 25, 35);
    options.escapeKeyTriggersCloseButton = true;
    options.useNativeTitleBar = true;
    options.resizable = true;
    options.launchAsync();
}

void MainComponent::timerCallback()
{
    auto* device = deviceManager.getCurrentAudioDevice();
    const juce::String deviceName = device != nullptr ? device->getName()
                                                       : "Chua co thiet bi am thanh";

    if (deviceName != lastAudioDeviceName)
    {
        lastAudioDeviceName = deviceName;
        deviceSelector.clear(juce::dontSendNotification);
        deviceSelector.addItem(deviceName, 1);
        deviceSelector.setSelectedId(1, juce::dontSendNotification);

        const bool ready = device != nullptr;
        connectionLabel.setText(ready ? "AM THANH SAN SANG  |  DA KET NOI"
                                      : "CHUA SAN SANG  |  KIEM TRA CAI DAT",
                                juce::dontSendNotification);
        connectionLabel.setColour(juce::Label::textColourId, ready ? green
                                                                    : juce::Colour(255, 110, 90));
    }

    repaint();
}
