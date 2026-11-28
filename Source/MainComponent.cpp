#include "MainComponent.h"
#include <cmath>
#include <functional>

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
        const auto outer = r.expanded(1.5f);

        // Deep shadow and a restrained cyan outer glow.
        g.setColour(juce::Colours::black.withAlpha(0.82f));
        g.fillRoundedRectangle(r.translated(0.0f, 5.0f), 13.0f);
        g.setColour(cyan.withAlpha(0.075f));
        g.drawRoundedRectangle(outer, 14.0f, 4.0f);

        // Brushed-metal body with a darker lower recess.
        g.setGradientFill(juce::ColourGradient(
            juce::Colour(43, 55, 70), r.getX(), r.getY(),
            juce::Colour(8, 13, 20), r.getX(), r.getBottom(), false));
        g.fillRoundedRectangle(r, 12.0f);
        g.setColour(juce::Colour(105, 128, 151).withAlpha(0.78f));
        g.drawRoundedRectangle(r.reduced(0.7f), 12.0f, 1.2f);
        g.setColour(juce::Colour(2, 6, 11).withAlpha(0.92f));
        g.drawRoundedRectangle(r.reduced(3.0f), 10.0f, 1.0f);

        // Header rail, cyan accent and fine separator.
        auto header = juce::Rectangle<float>(r.getX() + 2.0f, r.getY() + 2.0f,
                                              r.getWidth() - 4.0f, 41.0f);
        g.setGradientFill(juce::ColourGradient(
            juce::Colour(18, 29, 42), header.getX(), header.getY(),
            juce::Colour(5, 10, 17), header.getX(), header.getBottom(), false));
        g.fillRoundedRectangle(header, 9.0f);
        g.setColour(iconColour.withAlpha(0.9f));
        g.fillRoundedRectangle(r.getX() + 9.0f, r.getY() + 9.0f, 4.0f, 25.0f, 2.0f);
        g.setColour(iconColour.withAlpha(0.35f));
        g.drawHorizontalLine((int)(r.getY() + 43.0f), r.getX() + 10.0f, r.getRight() - 10.0f);

        g.setColour(bright);
        g.setFont(juce::FontOptions(16.0f, juce::Font::bold));
        g.drawFittedText(title, area.getX() + 22, area.getY() + 6,
                         area.getWidth() - 112, 32,
                         juce::Justification::centredLeft, 1, 0.78f);

        // Small illuminated state jewel and machined corner fasteners.
        g.setColour(green.withAlpha(0.18f));
        g.fillEllipse(r.getRight() - 30.0f, r.getY() + 12.0f, 16.0f, 16.0f);
        g.setColour(green);
        g.fillEllipse(r.getRight() - 25.0f, r.getY() + 17.0f, 6.0f, 6.0f);
        g.setColour(juce::Colour(120, 139, 157).withAlpha(0.55f));
        for (const auto p : { juce::Point<float>(r.getX() + 8.0f, r.getY() + 8.0f),
                              juce::Point<float>(r.getRight() - 8.0f, r.getBottom() - 8.0f) })
        {
            g.fillEllipse(p.x - 2.0f, p.y - 2.0f, 4.0f, 4.0f);
            g.setColour(juce::Colour(25, 34, 44));
            g.drawLine(p.x - 1.0f, p.y + 1.0f, p.x + 1.0f, p.y - 1.0f, 0.8f);
            g.setColour(juce::Colour(120, 139, 157).withAlpha(0.55f));
        }
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

    BiquadCoefficients makeBandPassCoefficients(float sampleRate, float frequency, float q)
    {
        const float safeRate = juce::jmax(8000.0f, sampleRate);
        const float f = juce::jlimit(500.0f, safeRate * 0.45f, frequency);
        const float safeQ = juce::jmax(0.1f, q);
        const float w0 = juce::MathConstants<float>::twoPi * f / safeRate;
        const float alpha = std::sin(w0) / (2.0f * safeQ);
        const float cosine = std::cos(w0);

        return normaliseBiquad(alpha, 0.0f, -alpha,
                               1.0f + alpha, -2.0f * cosine,
                               1.0f - alpha);
    }


class PitchCorrectionModuleContent final : public juce::Component, private juce::Timer
{
public:
    PitchCorrectionModuleContent(std::atomic<int>& songKey,
                                 std::atomic<int>& scale,
                                 std::atomic<float>& amount,
                                 std::atomic<float>& speed,
                                 std::atomic<int>& detectedMidi,
                                 std::atomic<int>& targetMidi,
                                 std::atomic<float>& deviationCents,
                                 std::atomic<bool>& enabled)
        : songKeyParameter(songKey), scaleParameter(scale),
          amountParameter(amount), speedParameter(speed),
          detectedMidiParameter(detectedMidi), targetMidiParameter(targetMidi),
          deviationCentsParameter(deviationCents), enabledParameter(enabled)
    {
        title.setText("PITCH CORRECTION", juce::dontSendNotification);
        title.setFont(juce::FontOptions(25.0f, juce::Font::bold));
        title.setColour(juce::Label::textColourId, juce::Colour(235, 243, 252));
        addAndMakeVisible(title);

        description.setText("Live vocal pitch tracking and scale target analysis.",
                            juce::dontSendNotification);
        description.setFont(juce::FontOptions(14.0f));
        description.setColour(juce::Label::textColourId, juce::Colour(153, 169, 187));
        addAndMakeVisible(description);

        keyLabel.setText("SONG KEY", juce::dontSendNotification);
        scaleLabel.setText("SCALE", juce::dontSendNotification);
        strengthLabel.setText("CORRECTION AMOUNT", juce::dontSendNotification);
        speedLabel.setText("CORRECTION SPEED", juce::dontSendNotification);
        for (auto* label : { &keyLabel, &scaleLabel, &strengthLabel, &speedLabel })
        {
            label->setFont(juce::FontOptions(12.0f, juce::Font::bold));
            label->setColour(juce::Label::textColourId, juce::Colour(210, 222, 235));
            addAndMakeVisible(*label);
        }

        const char* keys[] { "C", "C#", "D", "D#", "E", "F",
                             "F#", "G", "G#", "A", "A#", "B" };
        for (int i = 0; i < 12; ++i) keySelector.addItem(keys[i], i + 1);
        keySelector.setSelectedId(songKeyParameter.load() + 1, juce::dontSendNotification);
        keySelector.onChange = [this]
        {
            songKeyParameter.store(keySelector.getSelectedId() - 1);
        };
        addAndMakeVisible(keySelector);

        scaleSelector.addItem("Major", 1);
        scaleSelector.addItem("Natural Minor", 2);
        scaleSelector.addItem("Chromatic", 3);
        scaleSelector.setSelectedId(scaleParameter.load(), juce::dontSendNotification);
        scaleSelector.onChange = [this]
        {
            scaleParameter.store(scaleSelector.getSelectedId());
        };
        addAndMakeVisible(scaleSelector);

        strengthSlider.setSliderStyle(juce::Slider::LinearHorizontal);
        strengthSlider.setRange(0.0, 100.0, 1.0);
        strengthSlider.setValue(amountParameter.load(), juce::dontSendNotification);
        strengthSlider.setTextValueSuffix(" %");
        strengthSlider.onValueChange = [this]
        {
            amountParameter.store((float)strengthSlider.getValue());
        };
        addAndMakeVisible(strengthSlider);

        speedSlider.setSliderStyle(juce::Slider::LinearHorizontal);
        speedSlider.setRange(5.0, 100.0, 1.0);
        speedSlider.setValue(speedParameter.load(), juce::dontSendNotification);
        speedSlider.setTextValueSuffix(" %");
        speedSlider.onValueChange = [this]
        {
            speedParameter.store((float)speedSlider.getValue());
        };
        addAndMakeVisible(speedSlider);

        enableToggle.setClickingTogglesState(true);
        enableToggle.setToggleState(enabledParameter.load(), juce::dontSendNotification);
        enableToggle.setButtonText(enabledParameter.load()
            ? "PITCH CORRECTION: ON" : "PITCH CORRECTION: OFF");
        enableToggle.setColour(juce::ToggleButton::textColourId, juce::Colour(235, 243, 252));
        enableToggle.onClick = [this]
        {
            const bool enabledNow = enableToggle.getToggleState();
            enabledParameter.store(enabledNow);
            enableToggle.setButtonText(enabledNow
                ? "PITCH CORRECTION: ON" : "PITCH CORRECTION: OFF");
            status.setText(enabledNow
                ? "Experimental pitch shifting enabled. Use a dry, single vocal input."
                : "Pitch correction is off. Pitch tracking remains active.",
                juce::dontSendNotification);
        };
        addAndMakeVisible(enableToggle);

        detectedLabel.setFont(juce::FontOptions(17.0f, juce::Font::bold));
        detectedLabel.setColour(juce::Label::textColourId, juce::Colour(50, 213, 255));
        detectedLabel.setText("Detected note: --", juce::dontSendNotification);
        addAndMakeVisible(detectedLabel);

        targetLabel.setFont(juce::FontOptions(15.0f, juce::Font::bold));
        targetLabel.setColour(juce::Label::textColourId, juce::Colour(235, 243, 252));
        targetLabel.setText("Scale target: --", juce::dontSendNotification);
        addAndMakeVisible(targetLabel);

        status.setText("Experimental pitch shifting. Best with a dry, single vocal input.",
                       juce::dontSendNotification);
        status.setFont(juce::FontOptions(12.0f));
        status.setColour(juce::Label::textColourId, juce::Colour(255, 190, 90));
        status.setJustificationType(juce::Justification::centredLeft);
        addAndMakeVisible(status);

        setSize(660, 530);
        startTimerHz(12);
    }

    ~PitchCorrectionModuleContent() override { stopTimer(); }

    void paint(juce::Graphics& g) override
    {
        g.fillAll(juce::Colour(13, 19, 28));
        auto area = getLocalBounds().toFloat().reduced(1.0f);
        g.setColour(juce::Colour(74, 94, 116));
        g.drawRoundedRectangle(area, 12.0f, 1.0f);
        g.setColour(juce::Colour(28, 164, 255).withAlpha(0.18f));
        g.fillRoundedRectangle(18.0f, 18.0f, (float)getWidth() - 36.0f, 72.0f, 10.0f);
    }

    void resized() override
    {
        auto area = getLocalBounds().reduced(28);
        title.setBounds(area.removeFromTop(42));
        description.setBounds(area.removeFromTop(34));
        area.removeFromTop(16);
        const int half = (area.getWidth() - 24) / 2;
        auto rowOne = area.removeFromTop(76);
        auto left = rowOne.removeFromLeft(half);
        keyLabel.setBounds(left.removeFromTop(22));
        keySelector.setBounds(left.getX(), left.getY() + 5, left.getWidth(), 34);
        rowOne.removeFromLeft(24);
        scaleLabel.setBounds(rowOne.getX(), rowOne.getY(), rowOne.getWidth(), 22);
        scaleSelector.setBounds(rowOne.getX(), rowOne.getY() + 27, rowOne.getWidth(), 34);
        area.removeFromTop(10);
        strengthLabel.setBounds(area.removeFromTop(22));
        strengthSlider.setBounds(area.removeFromTop(42));
        area.removeFromTop(4);
        speedLabel.setBounds(area.removeFromTop(22));
        speedSlider.setBounds(area.removeFromTop(42));
        area.removeFromTop(10);
        detectedLabel.setBounds(area.removeFromTop(28));
        targetLabel.setBounds(area.removeFromTop(26));
        enableToggle.setBounds(area.removeFromTop(34));
        status.setBounds(area.removeFromTop(48));
    }

private:
    static juce::String noteName(int midi)
    {
        static const char* names[] { "C", "C#", "D", "D#", "E", "F",
                                     "F#", "G", "G#", "A", "A#", "B" };
        if (midi < 0) return "--";
        return juce::String(names[midi % 12]) + juce::String(midi / 12 - 1);
    }

    void timerCallback() override
    {
        const int detected = detectedMidiParameter.load();
        const int target = targetMidiParameter.load();
        if (detected < 0)
        {
            detectedLabel.setText("Detected note: --", juce::dontSendNotification);
            targetLabel.setText("Scale target: --", juce::dontSendNotification);
            return;
        }
        detectedLabel.setText("Detected note: " + noteName(detected),
                              juce::dontSendNotification);
        const float cents = deviationCentsParameter.load();
        targetLabel.setText("Nearest scale note: " + noteName(target)
                            + "  |  Offset: " + juce::String(cents, 1) + " cents",
                            juce::dontSendNotification);
    }

    std::atomic<int>& songKeyParameter;
    std::atomic<int>& scaleParameter;
    std::atomic<float>& amountParameter;
    std::atomic<float>& speedParameter;
    std::atomic<int>& detectedMidiParameter;
    std::atomic<int>& targetMidiParameter;
    std::atomic<float>& deviationCentsParameter;
    std::atomic<bool>& enabledParameter;
    juce::Label title, description, keyLabel, scaleLabel;
    juce::Label strengthLabel, speedLabel, detectedLabel, targetLabel, status;
    juce::ComboBox keySelector, scaleSelector;
    juce::Slider strengthSlider, speedSlider;
    juce::ToggleButton enableToggle { "PITCH CORRECTION: OFF" };
};

class VocalLevelingModuleContent final : public juce::Component
{
public:
    VocalLevelingModuleContent(std::atomic<bool>& enabled,
                               std::atomic<float>& targetDb,
                               std::atomic<float>& maxBoostDb,
                               std::atomic<float>& maxCutDb)
        : enabledParameter(enabled), targetParameter(targetDb),
          boostParameter(maxBoostDb), cutParameter(maxCutDb)
    {
        title.setText("VOCAL LEVELING", juce::dontSendNotification);
        title.setFont(juce::FontOptions(25.0f, juce::Font::bold));
        title.setColour(juce::Label::textColourId, juce::Colour(235, 243, 252));
        addAndMakeVisible(title);

        description.setText("Smooth vocal level control with limited gain riding.",
                            juce::dontSendNotification);
        description.setFont(juce::FontOptions(14.0f));
        description.setColour(juce::Label::textColourId, juce::Colour(153, 169, 187));
        addAndMakeVisible(description);

        configureSlider(targetLabel, targetSlider, "TARGET LEVEL", -30.0, -12.0,
                        targetParameter.load(), " dB",
                        [this](double v) { targetParameter.store((float)v); });
        configureSlider(boostLabel, boostSlider, "MAXIMUM BOOST", 0.0, 12.0,
                        boostParameter.load(), " dB",
                        [this](double v) { boostParameter.store((float)v); });
        configureSlider(cutLabel, cutSlider, "MAXIMUM REDUCTION", 0.0, 12.0,
                        cutParameter.load(), " dB",
                        [this](double v) { cutParameter.store((float)v); });

        enableToggle.setToggleState(enabledParameter.load(), juce::dontSendNotification);
        enableToggle.setButtonText(enabledParameter.load()
            ? "VOCAL LEVELING: ON" : "VOCAL LEVELING: OFF");
        enableToggle.setColour(juce::ToggleButton::textColourId,
                               juce::Colour(235, 243, 252));
        enableToggle.onClick = [this]
        {
            const bool on = enableToggle.getToggleState();
            enabledParameter.store(on);
            enableToggle.setButtonText(on ? "VOCAL LEVELING: ON"
                                          : "VOCAL LEVELING: OFF");
        };
        addAndMakeVisible(enableToggle);

        note.setText("Low-level pauses are protected to avoid lifting room noise.",
                     juce::dontSendNotification);
        note.setFont(juce::FontOptions(12.0f));
        note.setColour(juce::Label::textColourId, juce::Colour(153, 169, 187));
        addAndMakeVisible(note);
        setSize(620, 470);
    }

    void paint(juce::Graphics& g) override
    {
        g.fillAll(juce::Colour(13, 19, 28));
        auto area = getLocalBounds().toFloat().reduced(1.0f);
        g.setColour(juce::Colour(74, 94, 116));
        g.drawRoundedRectangle(area, 12.0f, 1.0f);
        g.setColour(juce::Colour(28, 164, 255).withAlpha(0.16f));
        g.fillRoundedRectangle(18.0f, 18.0f, (float)getWidth() - 36.0f, 72.0f, 10.0f);
    }

    void resized() override
    {
        auto area = getLocalBounds().reduced(28);
        title.setBounds(area.removeFromTop(42));
        description.setBounds(area.removeFromTop(34));
        area.removeFromTop(18);
        targetLabel.setBounds(area.removeFromTop(22));
        targetSlider.setBounds(area.removeFromTop(44));
        area.removeFromTop(10);
        boostLabel.setBounds(area.removeFromTop(22));
        boostSlider.setBounds(area.removeFromTop(44));
        area.removeFromTop(10);
        cutLabel.setBounds(area.removeFromTop(22));
        cutSlider.setBounds(area.removeFromTop(44));
        area.removeFromTop(16);
        enableToggle.setBounds(area.removeFromTop(34));
        note.setBounds(area.removeFromTop(34));
    }

private:
    void configureSlider(juce::Label& label, juce::Slider& slider,
                         const juce::String& caption, double minimum,
                         double maximum, double initial, const juce::String& suffix,
                         std::function<void(double)> onChange)
    {
        label.setText(caption, juce::dontSendNotification);
        label.setFont(juce::FontOptions(12.0f, juce::Font::bold));
        label.setColour(juce::Label::textColourId, juce::Colour(210, 222, 235));
        addAndMakeVisible(label);

        slider.setSliderStyle(juce::Slider::LinearHorizontal);
        slider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 72, 26);
        slider.setRange(minimum, maximum, 0.5);
        slider.setValue(initial, juce::dontSendNotification);
        slider.setTextValueSuffix(suffix);
        slider.setColour(juce::Slider::trackColourId, juce::Colour(28, 164, 255));
        slider.setColour(juce::Slider::textBoxTextColourId, juce::Colour(235, 243, 252));
        auto* sliderPointer = &slider;
        slider.onValueChange = [sliderPointer, onChange]
        {
            onChange(sliderPointer->getValue());
        };
        addAndMakeVisible(slider);
    }

    std::atomic<bool>& enabledParameter;
    std::atomic<float>& targetParameter;
    std::atomic<float>& boostParameter;
    std::atomic<float>& cutParameter;
    juce::Label title, description, targetLabel, boostLabel, cutLabel, note;
    juce::Slider targetSlider, boostSlider, cutSlider;
    juce::ToggleButton enableToggle { "VOCAL LEVELING: OFF" };
};


class WarmthBodyModuleContent final : public juce::Component
{
public:
    WarmthBodyModuleContent(std::atomic<bool>& enabled,
                            std::atomic<float>& warmth,
                            std::atomic<float>& body)
        : enabledParameter(enabled), warmthParameter(warmth), bodyParameter(body)
    {
        title.setText("WARMTH & BODY", juce::dontSendNotification);
        title.setFont(juce::FontOptions(25.0f, juce::Font::bold));
        title.setColour(juce::Label::textColourId, juce::Colour(235, 243, 252));
        addAndMakeVisible(title);

        description.setText("Add controlled warmth and vocal body without excessive low-end buildup.",
                            juce::dontSendNotification);
        description.setFont(juce::FontOptions(14.0f));
        description.setColour(juce::Label::textColourId, juce::Colour(153, 169, 187));
        addAndMakeVisible(description);

        configureSlider(warmthLabel, warmthSlider, "WARMTH", warmthParameter.load(),
                        [this](double value) { warmthParameter.store((float)value); });
        configureSlider(bodyLabel, bodySlider, "BODY", bodyParameter.load(),
                        [this](double value) { bodyParameter.store((float)value); });

        enableToggle.setToggleState(enabledParameter.load(), juce::dontSendNotification);
        updateToggleText();
        enableToggle.setColour(juce::ToggleButton::textColourId,
                               juce::Colour(235, 243, 252));
        enableToggle.onClick = [this]
        {
            enabledParameter.store(enableToggle.getToggleState());
            updateToggleText();
        };
        addAndMakeVisible(enableToggle);

        note.setText("Start near 50%. Higher values add low-frequency weight; reduce if the voice sounds boomy.",
                     juce::dontSendNotification);
        note.setFont(juce::FontOptions(12.0f));
        note.setColour(juce::Label::textColourId, juce::Colour(153, 169, 187));
        addAndMakeVisible(note);
        setSize(620, 390);
    }

    void paint(juce::Graphics& g) override
    {
        g.fillAll(juce::Colour(13, 19, 28));
        auto area = getLocalBounds().toFloat().reduced(1.0f);
        g.setColour(juce::Colour(74, 94, 116));
        g.drawRoundedRectangle(area, 12.0f, 1.0f);
        g.setColour(juce::Colour(28, 164, 255).withAlpha(0.16f));
        g.fillRoundedRectangle(18.0f, 18.0f, (float)getWidth() - 36.0f, 72.0f, 10.0f);
    }

    void resized() override
    {
        auto area = getLocalBounds().reduced(28);
        title.setBounds(area.removeFromTop(42));
        description.setBounds(area.removeFromTop(42));
        area.removeFromTop(14);
        warmthLabel.setBounds(area.removeFromTop(22));
        warmthSlider.setBounds(area.removeFromTop(44));
        area.removeFromTop(12);
        bodyLabel.setBounds(area.removeFromTop(22));
        bodySlider.setBounds(area.removeFromTop(44));
        area.removeFromTop(14);
        enableToggle.setBounds(area.removeFromTop(34));
        note.setBounds(area.removeFromTop(42));
    }

private:
    void configureSlider(juce::Label& label, juce::Slider& slider,
                         const juce::String& caption, double initial,
                         std::function<void(double)> onChange)
    {
        label.setText(caption, juce::dontSendNotification);
        label.setFont(juce::FontOptions(12.0f, juce::Font::bold));
        label.setColour(juce::Label::textColourId, juce::Colour(210, 222, 235));
        addAndMakeVisible(label);

        slider.setSliderStyle(juce::Slider::LinearHorizontal);
        slider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 72, 26);
        slider.setRange(0.0, 100.0, 1.0);
        slider.setValue(initial, juce::dontSendNotification);
        slider.setTextValueSuffix(" %");
        slider.setColour(juce::Slider::trackColourId, juce::Colour(28, 164, 255));
        slider.setColour(juce::Slider::textBoxTextColourId, juce::Colour(235, 243, 252));
        auto* sliderPointer = &slider;
        slider.onValueChange = [sliderPointer, onChange]
        {
            onChange(sliderPointer->getValue());
        };
        addAndMakeVisible(slider);
    }

    void updateToggleText()
    {
        enableToggle.setButtonText(enabledParameter.load()
            ? "WARMTH & BODY: ON" : "WARMTH & BODY: OFF");
    }

    std::atomic<bool>& enabledParameter;
    std::atomic<float>& warmthParameter;
    std::atomic<float>& bodyParameter;
    juce::Label title, description, warmthLabel, bodyLabel, note;
    juce::Slider warmthSlider, bodySlider;
    juce::ToggleButton enableToggle { "WARMTH & BODY: OFF" };
};


class VocalClarityModuleContent final : public juce::Component
{
public:
    VocalClarityModuleContent(std::atomic<bool>& enabled,
                              std::atomic<float>& clarity,
                              std::atomic<float>& air)
        : enabledParameter(enabled), clarityParameter(clarity), airParameter(air)
    {
        title.setText("VOCAL CLARITY", juce::dontSendNotification);
        title.setFont(juce::FontOptions(25.0f, juce::Font::bold));
        title.setColour(juce::Label::textColourId, juce::Colour(235, 243, 252));
        addAndMakeVisible(title);

        description.setText("Bring vocal words forward with controlled presence and air.",
                            juce::dontSendNotification);
        description.setFont(juce::FontOptions(14.0f));
        description.setColour(juce::Label::textColourId, juce::Colour(153, 169, 187));
        addAndMakeVisible(description);

        configureSlider(clarityLabel, claritySlider, "CLARITY", clarityParameter.load(),
                        [this](double value) { clarityParameter.store((float)value); });
        configureSlider(airLabel, airSlider, "AIR", airParameter.load(),
                        [this](double value) { airParameter.store((float)value); });

        enableToggle.setToggleState(enabledParameter.load(), juce::dontSendNotification);
        updateToggleText();
        enableToggle.setColour(juce::ToggleButton::textColourId,
                               juce::Colour(235, 243, 252));
        enableToggle.onClick = [this]
        {
            enabledParameter.store(enableToggle.getToggleState());
            updateToggleText();
        };
        addAndMakeVisible(enableToggle);

        note.setText("Start near 50%. Increase gently; too much presence or air can make S sounds sharp.",
                     juce::dontSendNotification);
        note.setFont(juce::FontOptions(12.0f));
        note.setColour(juce::Label::textColourId, juce::Colour(153, 169, 187));
        addAndMakeVisible(note);
        setSize(620, 390);
    }

    void paint(juce::Graphics& g) override
    {
        g.fillAll(juce::Colour(13, 19, 28));
        auto area = getLocalBounds().toFloat().reduced(1.0f);
        g.setColour(juce::Colour(74, 94, 116));
        g.drawRoundedRectangle(area, 12.0f, 1.0f);
        g.setColour(juce::Colour(50, 213, 255).withAlpha(0.15f));
        g.fillRoundedRectangle(18.0f, 18.0f, (float)getWidth() - 36.0f, 72.0f, 10.0f);
    }

    void resized() override
    {
        auto area = getLocalBounds().reduced(28);
        title.setBounds(area.removeFromTop(42));
        description.setBounds(area.removeFromTop(42));
        area.removeFromTop(14);
        clarityLabel.setBounds(area.removeFromTop(22));
        claritySlider.setBounds(area.removeFromTop(44));
        area.removeFromTop(12);
        airLabel.setBounds(area.removeFromTop(22));
        airSlider.setBounds(area.removeFromTop(44));
        area.removeFromTop(14);
        enableToggle.setBounds(area.removeFromTop(34));
        note.setBounds(area.removeFromTop(42));
    }

private:
    void configureSlider(juce::Label& label, juce::Slider& slider,
                         const juce::String& caption, double initial,
                         std::function<void(double)> onChange)
    {
        label.setText(caption, juce::dontSendNotification);
        label.setFont(juce::FontOptions(12.0f, juce::Font::bold));
        label.setColour(juce::Label::textColourId, juce::Colour(210, 222, 235));
        addAndMakeVisible(label);

        slider.setSliderStyle(juce::Slider::LinearHorizontal);
        slider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 72, 26);
        slider.setRange(0.0, 100.0, 1.0);
        slider.setValue(initial, juce::dontSendNotification);
        slider.setTextValueSuffix(" %");
        slider.setColour(juce::Slider::trackColourId, juce::Colour(50, 213, 255));
        slider.setColour(juce::Slider::textBoxTextColourId, juce::Colour(235, 243, 252));
        auto* sliderPointer = &slider;
        slider.onValueChange = [sliderPointer, onChange]
        {
            onChange(sliderPointer->getValue());
        };
        addAndMakeVisible(slider);
    }

    void updateToggleText()
    {
        enableToggle.setButtonText(enabledParameter.load()
            ? "VOCAL CLARITY: ON" : "VOCAL CLARITY: OFF");
    }

    std::atomic<bool>& enabledParameter;
    std::atomic<float>& clarityParameter;
    std::atomic<float>& airParameter;
    juce::Label title, description, clarityLabel, airLabel, note;
    juce::Slider claritySlider, airSlider;
    juce::ToggleButton enableToggle { "VOCAL CLARITY: OFF" };
};

class BreathControlModuleContent final : public juce::Component
{
public:
    BreathControlModuleContent(std::atomic<bool>& enabled,
                               std::atomic<float>& sensitivity,
                               std::atomic<float>& reduction)
        : enabledParameter(enabled), sensitivityParameter(sensitivity),
          reductionParameter(reduction)
    {
        title.setText("BREATH CONTROL", juce::dontSendNotification);
        title.setFont(juce::FontOptions(25.0f, juce::Font::bold));
        title.setColour(juce::Label::textColourId, juce::Colour(235, 243, 252));
        addAndMakeVisible(title);

        description.setText("Reduce distracting breath noise while keeping natural vocal detail.",
                            juce::dontSendNotification);
        description.setFont(juce::FontOptions(14.0f));
        description.setColour(juce::Label::textColourId, juce::Colour(153, 169, 187));
        addAndMakeVisible(description);

        configureSlider(sensitivityLabel, sensitivitySlider, "SENSITIVITY",
                        sensitivityParameter.load(),
                        [this](double value) { sensitivityParameter.store((float)value); });
        configureSlider(reductionLabel, reductionSlider, "BREATH REDUCTION",
                        reductionParameter.load(),
                        [this](double value) { reductionParameter.store((float)value); });

        enableToggle.setToggleState(enabledParameter.load(), juce::dontSendNotification);
        updateToggleText();
        enableToggle.setColour(juce::ToggleButton::textColourId,
                               juce::Colour(235, 243, 252));
        enableToggle.onClick = [this]
        {
            enabledParameter.store(enableToggle.getToggleState());
            updateToggleText();
        };
        addAndMakeVisible(enableToggle);

        note.setText("Start with low reduction. Excessive settings may soften quiet consonants and vocal air.",
                     juce::dontSendNotification);
        note.setFont(juce::FontOptions(12.0f));
        note.setColour(juce::Label::textColourId, juce::Colour(153, 169, 187));
        addAndMakeVisible(note);
        setSize(620, 390);
    }

    void paint(juce::Graphics& g) override
    {
        g.fillAll(juce::Colour(13, 19, 28));
        auto area = getLocalBounds().toFloat().reduced(1.0f);
        g.setColour(juce::Colour(74, 94, 116));
        g.drawRoundedRectangle(area, 12.0f, 1.0f);
        g.setColour(juce::Colour(125, 145, 205).withAlpha(0.16f));
        g.fillRoundedRectangle(18.0f, 18.0f, (float)getWidth() - 36.0f, 72.0f, 10.0f);
    }

    void resized() override
    {
        auto area = getLocalBounds().reduced(28);
        title.setBounds(area.removeFromTop(42));
        description.setBounds(area.removeFromTop(42));
        area.removeFromTop(14);
        sensitivityLabel.setBounds(area.removeFromTop(22));
        sensitivitySlider.setBounds(area.removeFromTop(44));
        area.removeFromTop(12);
        reductionLabel.setBounds(area.removeFromTop(22));
        reductionSlider.setBounds(area.removeFromTop(44));
        area.removeFromTop(14);
        enableToggle.setBounds(area.removeFromTop(34));
        note.setBounds(area.removeFromTop(42));
    }

private:
    void configureSlider(juce::Label& label, juce::Slider& slider,
                         const juce::String& caption, double initial,
                         std::function<void(double)> onChange)
    {
        label.setText(caption, juce::dontSendNotification);
        label.setFont(juce::FontOptions(12.0f, juce::Font::bold));
        label.setColour(juce::Label::textColourId, juce::Colour(210, 222, 235));
        addAndMakeVisible(label);

        slider.setSliderStyle(juce::Slider::LinearHorizontal);
        slider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 72, 26);
        slider.setRange(0.0, 100.0, 1.0);
        slider.setValue(initial, juce::dontSendNotification);
        slider.setTextValueSuffix(" %");
        slider.setColour(juce::Slider::trackColourId, juce::Colour(125, 145, 205));
        slider.setColour(juce::Slider::textBoxTextColourId, juce::Colour(235, 243, 252));
        auto* sliderPointer = &slider;
        slider.onValueChange = [sliderPointer, onChange]
        {
            onChange(sliderPointer->getValue());
        };
        addAndMakeVisible(slider);
    }

    void updateToggleText()
    {
        enableToggle.setButtonText(enabledParameter.load()
            ? "BREATH CONTROL: ON" : "BREATH CONTROL: OFF");
    }

    std::atomic<bool>& enabledParameter;
    std::atomic<float>& sensitivityParameter;
    std::atomic<float>& reductionParameter;
    juce::Label title, description, sensitivityLabel, reductionLabel, note;
    juce::Slider sensitivitySlider, reductionSlider;
    juce::ToggleButton enableToggle { "BREATH CONTROL: OFF" };
};



class VocalExciterModuleContent final : public juce::Component
{
public:
    VocalExciterModuleContent(std::atomic<bool>& enabled,
                              std::atomic<float>& amount,
                              std::atomic<float>& mix)
        : enabledParameter(enabled), amountParameter(amount), mixParameter(mix)
    {
        title.setText("VOCAL EXCITER", juce::dontSendNotification);
        title.setFont(juce::FontOptions(25.0f, juce::Font::bold));
        title.setColour(juce::Label::textColourId, juce::Colour(235, 243, 252));
        addAndMakeVisible(title);

        description.setText("Add controlled upper harmonics for a brighter, more lively vocal.",
                            juce::dontSendNotification);
        description.setFont(juce::FontOptions(14.0f));
        description.setColour(juce::Label::textColourId, juce::Colour(153, 169, 187));
        addAndMakeVisible(description);

        configureSlider(amountLabel, amountSlider, "EXCITER AMOUNT",
                        amountParameter.load(),
                        [this](double value) { amountParameter.store((float)value); });
        configureSlider(mixLabel, mixSlider, "EFFECT MIX",
                        mixParameter.load(),
                        [this](double value) { mixParameter.store((float)value); });

        enableToggle.setToggleState(enabledParameter.load(), juce::dontSendNotification);
        updateToggleText();
        enableToggle.setColour(juce::ToggleButton::textColourId,
                               juce::Colour(235, 243, 252));
        enableToggle.onClick = [this]
        {
            enabledParameter.store(enableToggle.getToggleState());
            updateToggleText();
        };
        addAndMakeVisible(enableToggle);

        note.setText("Use a low amount first. Too much exciter can make S sounds and hiss harsh.",
                     juce::dontSendNotification);
        note.setFont(juce::FontOptions(12.0f));
        note.setColour(juce::Label::textColourId, juce::Colour(153, 169, 187));
        addAndMakeVisible(note);
        setSize(620, 390);
    }

    void paint(juce::Graphics& g) override
    {
        g.fillAll(juce::Colour(13, 19, 28));
        auto area = getLocalBounds().toFloat().reduced(1.0f);
        g.setColour(juce::Colour(74, 94, 116));
        g.drawRoundedRectangle(area, 12.0f, 1.0f);
        g.setColour(juce::Colour(175, 105, 220).withAlpha(0.16f));
        g.fillRoundedRectangle(18.0f, 18.0f, (float)getWidth() - 36.0f, 72.0f, 10.0f);
    }

    void resized() override
    {
        auto area = getLocalBounds().reduced(28);
        title.setBounds(area.removeFromTop(42));
        description.setBounds(area.removeFromTop(42));
        area.removeFromTop(14);
        amountLabel.setBounds(area.removeFromTop(22));
        amountSlider.setBounds(area.removeFromTop(44));
        area.removeFromTop(12);
        mixLabel.setBounds(area.removeFromTop(22));
        mixSlider.setBounds(area.removeFromTop(44));
        area.removeFromTop(14);
        enableToggle.setBounds(area.removeFromTop(34));
        note.setBounds(area.removeFromTop(42));
    }

private:
    void configureSlider(juce::Label& label, juce::Slider& slider,
                         const juce::String& caption, double initial,
                         std::function<void(double)> onChange)
    {
        label.setText(caption, juce::dontSendNotification);
        label.setFont(juce::FontOptions(12.0f, juce::Font::bold));
        label.setColour(juce::Label::textColourId, juce::Colour(210, 222, 235));
        addAndMakeVisible(label);

        slider.setSliderStyle(juce::Slider::LinearHorizontal);
        slider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 72, 26);
        slider.setRange(0.0, 100.0, 1.0);
        slider.setValue(initial, juce::dontSendNotification);
        slider.setTextValueSuffix(" %");
        slider.setColour(juce::Slider::trackColourId, juce::Colour(175, 105, 220));
        slider.setColour(juce::Slider::textBoxTextColourId, juce::Colour(235, 243, 252));
        auto* sliderPointer = &slider;
        slider.onValueChange = [sliderPointer, onChange]
        {
            onChange(sliderPointer->getValue());
        };
        addAndMakeVisible(slider);
    }

    void updateToggleText()
    {
        enableToggle.setButtonText(enabledParameter.load()
            ? "VOCAL EXCITER: ON" : "VOCAL EXCITER: OFF");
    }

    std::atomic<bool>& enabledParameter;
    std::atomic<float>& amountParameter;
    std::atomic<float>& mixParameter;
    juce::Label title, description, amountLabel, mixLabel, note;
    juce::Slider amountSlider, mixSlider;
    juce::ToggleButton enableToggle { "VOCAL EXCITER: OFF" };
};




class VocalSmoothModuleContent final : public juce::Component
{
public:
    VocalSmoothModuleContent(std::atomic<bool>& enabled,
                             std::atomic<float>& sensitivity,
                             std::atomic<float>& intensity)
        : enabledParameter(enabled), sensitivityParameter(sensitivity),
          intensityParameter(intensity)
    {
        title.setText("VOCAL SMOOTH", juce::dontSendNotification);
        title.setFont(juce::FontOptions(25.0f, juce::Font::bold));
        title.setColour(juce::Label::textColourId, juce::Colour(235, 243, 252));
        addAndMakeVisible(title);

        description.setText("Gently control harsh upper-mid vocal resonances while keeping detail.",
                            juce::dontSendNotification);
        description.setFont(juce::FontOptions(14.0f));
        description.setColour(juce::Label::textColourId, juce::Colour(153, 169, 187));
        addAndMakeVisible(description);

        configureSlider(sensitivityLabel, sensitivitySlider, "SENSITIVITY",
                         sensitivityParameter.load(),
                         [this](double value) { sensitivityParameter.store((float)value); });
        configureSlider(intensityLabel, intensitySlider, "SMOOTHING INTENSITY",
                         intensityParameter.load(),
                         [this](double value) { intensityParameter.store((float)value); });

        enableToggle.setToggleState(enabledParameter.load(), juce::dontSendNotification);
        updateToggleText();
        enableToggle.setColour(juce::ToggleButton::textColourId,
                               juce::Colour(235, 243, 252));
        enableToggle.onClick = [this]
        {
            enabledParameter.store(enableToggle.getToggleState());
            updateToggleText();
        };
        addAndMakeVisible(enableToggle);

        note.setText("Start with low intensity. Excessive reduction can make vocals sound dull.",
                     juce::dontSendNotification);
        note.setFont(juce::FontOptions(12.0f));
        note.setColour(juce::Label::textColourId, juce::Colour(153, 169, 187));
        addAndMakeVisible(note);
        setSize(620, 390);
    }

    void paint(juce::Graphics& g) override
    {
        g.fillAll(juce::Colour(13, 19, 28));
        auto area = getLocalBounds().toFloat().reduced(1.0f);
        g.setColour(juce::Colour(74, 94, 116));
        g.drawRoundedRectangle(area, 12.0f, 1.0f);
        g.setColour(juce::Colour(105, 155, 210).withAlpha(0.16f));
        g.fillRoundedRectangle(18.0f, 18.0f, (float)getWidth() - 36.0f, 72.0f, 10.0f);
    }

    void resized() override
    {
        auto area = getLocalBounds().reduced(28);
        title.setBounds(area.removeFromTop(42));
        description.setBounds(area.removeFromTop(42));
        area.removeFromTop(14);
        sensitivityLabel.setBounds(area.removeFromTop(22));
        sensitivitySlider.setBounds(area.removeFromTop(44));
        area.removeFromTop(12);
        intensityLabel.setBounds(area.removeFromTop(22));
        intensitySlider.setBounds(area.removeFromTop(44));
        area.removeFromTop(14);
        enableToggle.setBounds(area.removeFromTop(34));
        note.setBounds(area.removeFromTop(42));
    }

private:
    void configureSlider(juce::Label& label, juce::Slider& slider,
                         const juce::String& caption, double initial,
                         std::function<void(double)> onChange)
    {
        label.setText(caption, juce::dontSendNotification);
        label.setFont(juce::FontOptions(12.0f, juce::Font::bold));
        label.setColour(juce::Label::textColourId, juce::Colour(210, 222, 235));
        addAndMakeVisible(label);

        slider.setSliderStyle(juce::Slider::LinearHorizontal);
        slider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 72, 26);
        slider.setRange(0.0, 100.0, 1.0);
        slider.setValue(initial, juce::dontSendNotification);
        slider.setTextValueSuffix(" %");
        slider.setColour(juce::Slider::trackColourId, juce::Colour(105, 155, 210));
        slider.setColour(juce::Slider::textBoxTextColourId, juce::Colour(235, 243, 252));
        auto* sliderPointer = &slider;
        slider.onValueChange = [sliderPointer, onChange]
        {
            onChange(sliderPointer->getValue());
        };
        addAndMakeVisible(slider);
    }

    void updateToggleText()
    {
        enableToggle.setButtonText(enabledParameter.load()
            ? "VOCAL SMOOTH: ON" : "VOCAL SMOOTH: OFF");
    }

    std::atomic<bool>& enabledParameter;
    std::atomic<float>& sensitivityParameter;
    std::atomic<float>& intensityParameter;
    juce::Label title, description, sensitivityLabel, intensityLabel, note;
    juce::Slider sensitivitySlider, intensitySlider;
    juce::ToggleButton enableToggle { "VOCAL SMOOTH: OFF" };
};


class VocalLimiterModuleContent final : public juce::Component
{
public:
    VocalLimiterModuleContent(std::atomic<bool>& enabled,
                              std::atomic<float>& ceiling,
                              std::atomic<float>& release)
        : enabledParameter(enabled), ceilingParameter(ceiling),
          releaseParameter(release)
    {
        title.setText("VOCAL LIMITER", juce::dontSendNotification);
        title.setFont(juce::FontOptions(25.0f, juce::Font::bold));
        title.setColour(juce::Label::textColourId, juce::Colour(235, 243, 252));
        addAndMakeVisible(title);

        description.setText("Control sudden vocal peaks and help prevent output clipping.",
                            juce::dontSendNotification);
        description.setFont(juce::FontOptions(14.0f));
        description.setColour(juce::Label::textColourId, juce::Colour(153, 169, 187));
        addAndMakeVisible(description);

        configureSlider(ceilingLabel, ceilingSlider, "OUTPUT CEILING",
                        ceilingParameter.load(), -6.0, -0.1, 0.1, " dBFS",
                        [this](double value) { ceilingParameter.store((float)value); });
        configureSlider(releaseLabel, releaseSlider, "RELEASE TIME",
                        releaseParameter.load(), 20.0, 300.0, 1.0, " ms",
                        [this](double value) { releaseParameter.store((float)value); });

        enableToggle.setToggleState(enabledParameter.load(), juce::dontSendNotification);
        updateToggleText();
        enableToggle.setColour(juce::ToggleButton::textColourId,
                               juce::Colour(235, 243, 252));
        enableToggle.onClick = [this]
        {
            enabledParameter.store(enableToggle.getToggleState());
            updateToggleText();
        };
        addAndMakeVisible(enableToggle);

        note.setText("Start at -1.0 dBFS and 80 ms. Sample-peak limiter; no look-ahead.",
                     juce::dontSendNotification);
        note.setFont(juce::FontOptions(12.0f));
        note.setColour(juce::Label::textColourId, juce::Colour(153, 169, 187));
        addAndMakeVisible(note);
        setSize(620, 390);
    }

    void paint(juce::Graphics& g) override
    {
        g.fillAll(juce::Colour(13, 19, 28));
        auto area = getLocalBounds().toFloat().reduced(1.0f);
        g.setColour(juce::Colour(74, 94, 116));
        g.drawRoundedRectangle(area, 12.0f, 1.0f);
        g.setColour(juce::Colour(80, 155, 220).withAlpha(0.16f));
        g.fillRoundedRectangle(18.0f, 18.0f, (float)getWidth() - 36.0f, 72.0f, 10.0f);
    }

    void resized() override
    {
        auto area = getLocalBounds().reduced(28);
        title.setBounds(area.removeFromTop(42));
        description.setBounds(area.removeFromTop(42));
        area.removeFromTop(14);
        ceilingLabel.setBounds(area.removeFromTop(22));
        ceilingSlider.setBounds(area.removeFromTop(44));
        area.removeFromTop(12);
        releaseLabel.setBounds(area.removeFromTop(22));
        releaseSlider.setBounds(area.removeFromTop(44));
        area.removeFromTop(14);
        enableToggle.setBounds(area.removeFromTop(34));
        note.setBounds(area.removeFromTop(48));
    }

private:
    void configureSlider(juce::Label& label, juce::Slider& slider,
                         const juce::String& caption, double initial,
                         double minimum, double maximum, double interval,
                         const juce::String& suffix,
                         std::function<void(double)> onChange)
    {
        label.setText(caption, juce::dontSendNotification);
        label.setFont(juce::FontOptions(12.0f, juce::Font::bold));
        label.setColour(juce::Label::textColourId, juce::Colour(210, 222, 235));
        addAndMakeVisible(label);

        slider.setSliderStyle(juce::Slider::LinearHorizontal);
        slider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 82, 26);
        slider.setRange(minimum, maximum, interval);
        slider.setValue(initial, juce::dontSendNotification);
        slider.setTextValueSuffix(suffix);
        slider.setColour(juce::Slider::trackColourId, juce::Colour(80, 155, 220));
        slider.setColour(juce::Slider::textBoxTextColourId, juce::Colour(235, 243, 252));
        auto* sliderPointer = &slider;
        slider.onValueChange = [sliderPointer, onChange]
        {
            onChange(sliderPointer->getValue());
        };
        addAndMakeVisible(slider);
    }

    void updateToggleText()
    {
        enableToggle.setButtonText(enabledParameter.load()
            ? "VOCAL LIMITER: ON" : "VOCAL LIMITER: OFF");
    }

    std::atomic<bool>& enabledParameter;
    std::atomic<float>& ceilingParameter;
    std::atomic<float>& releaseParameter;
    juce::Label title, description, ceilingLabel, releaseLabel, note;
    juce::Slider ceilingSlider, releaseSlider;
    juce::ToggleButton enableToggle { "VOCAL LIMITER: OFF" };
};


class PlannedVocalModuleContent final : public juce::Component
{
public:
    explicit PlannedVocalModuleContent(const juce::String& moduleName)
    {
        title.setText(moduleName, juce::dontSendNotification);
        title.setFont(juce::FontOptions(24.0f, juce::Font::bold));
        title.setColour(juce::Label::textColourId, juce::Colour(235, 243, 252));
        addAndMakeVisible(title);

        description.setText("This module has its own window. Audio processing and controls are not connected yet.",
                            juce::dontSendNotification);
        description.setFont(juce::FontOptions(14.0f));
        description.setColour(juce::Label::textColourId, juce::Colour(153, 169, 187));
        description.setJustificationType(juce::Justification::centredLeft);
        addAndMakeVisible(description);

        setSize(560, 250);
    }

    void paint(juce::Graphics& g) override
    {
        g.fillAll(juce::Colour(13, 19, 28));
        g.setColour(juce::Colour(74, 94, 116));
        g.drawRoundedRectangle(getLocalBounds().toFloat().reduced(1.0f), 12.0f, 1.0f);
    }

    void resized() override
    {
        auto area = getLocalBounds().reduced(28);
        title.setBounds(area.removeFromTop(48));
        area.removeFromTop(18);
        description.setBounds(area.removeFromTop(70));
    }

private:
    juce::Label title, description;
};
}

void StudioToggleLookAndFeel::drawToggleButton(
    juce::Graphics& g, juce::ToggleButton& button,
    bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown)
{
    juce::ignoreUnused(shouldDrawButtonAsDown);
    const auto bounds = button.getLocalBounds().toFloat();
    const bool enabled = button.getToggleState();
    const bool wide = bounds.getWidth() > 110.0f;

    if (wide)
    {
        g.setColour(bright);
        g.setFont(juce::FontOptions(11.5f, juce::Font::bold));
        g.drawText(juce::String::fromUTF8("THEO DÕI ÂM THANH"),
                   bounds.withTrimmedRight(52.0f), juce::Justification::centredLeft);
    }

    const float pillW = wide ? 42.0f : juce::jmin(56.0f, bounds.getWidth() - 4.0f);
    const float pillH = 22.0f;
    const float pillX = wide ? bounds.getRight() - pillW - 2.0f
                             : (bounds.getWidth() - pillW) * 0.5f;
    const float pillY = (bounds.getHeight() - pillH) * 0.5f;
    const auto pill = juce::Rectangle<float>(pillX, pillY, pillW, pillH);

    g.setColour(enabled ? juce::Colour(13, 111, 222)
                        : juce::Colour(42, 53, 67));
    g.fillRoundedRectangle(pill, pillH * 0.5f);
    g.setColour(enabled ? cyan.withAlpha(0.8f)
                        : juce::Colour(105, 121, 138));
    g.drawRoundedRectangle(pill, pillH * 0.5f, 1.0f);

    const float knobD = 16.0f;
    const float knobX = enabled ? pill.getRight() - knobD - 3.0f
                                : pill.getX() + 3.0f;
    g.setColour(juce::Colours::white.withAlpha(0.96f));
    g.fillEllipse(knobX, pill.getY() + 3.0f, knobD, knobD);

    if (!wide)
    {
        g.setColour(juce::Colours::white);
        g.setFont(juce::FontOptions(8.5f, juce::Font::bold));
        g.drawText(enabled ? "ON" : "OFF",
                   pill.withTrimmedLeft(enabled ? 2.0f : 18.0f)
                       .withTrimmedRight(enabled ? 18.0f : 2.0f),
                   juce::Justification::centred);
    }

    if (shouldDrawButtonAsHighlighted)
    {
        g.setColour(cyan.withAlpha(0.12f));
        g.drawRoundedRectangle(bounds.reduced(1.0f), 5.0f, 1.0f);
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

    // Outer shadow, illuminated bezel and engraved scale.
    g.setColour(juce::Colours::black.withAlpha(0.92f));
    g.fillEllipse(cx - radius - 5.0f, cy - radius - 5.0f,
                  diameter + 10.0f, diameter + 10.0f);
    g.setColour(cyan.withAlpha(0.13f));
    g.drawEllipse(cx - radius - 2.0f, cy - radius - 2.0f,
                  diameter + 4.0f, diameter + 4.0f, 3.0f);
    g.setColour(juce::Colour(117, 143, 166));
    g.drawEllipse(cx - radius, cy - radius, diameter, diameter, 2.0f);

    juce::Path track;
    track.addCentredArc(cx, cy, radius - 4.0f, radius - 4.0f, 0.0f,
                        startAngle, endAngle, true);
    g.setColour(juce::Colour(26, 40, 55));
    g.strokePath(track, juce::PathStrokeType(5.5f));
    juce::Path active;
    active.addCentredArc(cx, cy, radius - 4.0f, radius - 4.0f, 0.0f,
                         startAngle, angle, true);
    g.setColour(cyan.withAlpha(0.20f));
    g.strokePath(active, juce::PathStrokeType(8.0f));
    g.setColour(cyan);
    g.strokePath(active, juce::PathStrokeType(4.0f,
        juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

    for (int i = 0; i <= 10; ++i)
    {
        const float a = startAngle + (endAngle - startAngle) * (float)i / 10.0f;
        const float inner = radius - 10.0f;
        const float outer = radius - 5.0f;
        g.setColour(i <= (int)std::round(sliderPos * 10.0f)
            ? cyan.withAlpha(0.95f) : juce::Colour(111, 132, 151).withAlpha(0.65f));
        g.drawLine(cx + std::cos(a) * inner, cy + std::sin(a) * inner,
                   cx + std::cos(a) * outer, cy + std::sin(a) * outer, 1.5f);
    }

    // Machined silver knob with a dark inset and subtle top highlight.
    juce::ColourGradient metal(juce::Colour(244, 248, 251), cx - radius * 0.3f,
        cy - radius * 0.72f, juce::Colour(55, 69, 84), cx + radius * 0.25f,
        cy + radius * 0.72f, false);
    g.setGradientFill(metal);
    g.fillEllipse(cx - radius * 0.70f, cy - radius * 0.70f,
                  radius * 1.40f, radius * 1.40f);
    g.setColour(juce::Colour(8, 15, 23));
    g.drawEllipse(cx - radius * 0.70f, cy - radius * 0.70f,
                  radius * 1.40f, radius * 1.40f, 2.2f);
    g.setColour(juce::Colour(255, 255, 255).withAlpha(0.24f));
    juce::Path highlightArc;
    highlightArc.addCentredArc(cx, cy, radius * 0.59f, radius * 0.59f,
                               0.0f, 3.65f, 5.75f, true);
    g.strokePath(highlightArc, juce::PathStrokeType(1.4f));

    // Pointer and centre cap.
    g.setColour(juce::Colour(18, 29, 41));
    g.drawLine(cx, cy, cx + std::cos(angle) * radius * 0.48f,
               cy + std::sin(angle) * radius * 0.48f, 4.0f);
    g.setColour(juce::Colour(242, 247, 251));
    g.fillEllipse(cx - 4.0f, cy - 4.0f, 8.0f, 8.0f);
    g.setColour(cyan.withAlpha(0.75f));
    g.drawEllipse(cx - 4.0f, cy - 4.0f, 8.0f, 8.0f, 1.0f);
}

MainComponent::MainComponent()
{

    titleLabel.setText("Voice Vocal Studio", juce::dontSendNotification);
    titleLabel.setFont(juce::FontOptions(38.0f, juce::Font::bold | juce::Font::italic));
    titleLabel.setColour(juce::Label::textColourId, bright);
    addAndMakeVisible(titleLabel);

    subtitleLabel.setText(juce::String::fromUTF8("Hát hay hơn - Truyền cảm hơn"), juce::dontSendNotification);
    subtitleLabel.setFont(juce::FontOptions(14.0f, juce::Font::italic));
    subtitleLabel.setColour(juce::Label::textColourId, muted);
    addAndMakeVisible(subtitleLabel);

    deviceLabel.setText(juce::String::fromUTF8("THIẾT BỊ PHÁT ÂM THANH"), juce::dontSendNotification);
    deviceLabel.setFont(juce::FontOptions(10.0f, juce::Font::bold));
    deviceLabel.setColour(juce::Label::textColourId, muted);
    addAndMakeVisible(deviceLabel);

    deviceSelector.setSelectedId(0, juce::dontSendNotification);
    deviceSelector.setEnabled(true);
    deviceSelector.onChange = [this] { selectAudioOutputDevice(); };
    deviceSelector.setColour(juce::ComboBox::backgroundColourId, juce::Colour(12, 18, 27));
    deviceSelector.setColour(juce::ComboBox::textColourId, bright);
    deviceSelector.setColour(juce::ComboBox::outlineColourId, edge);
    addAndMakeVisible(deviceSelector);

    connectionLabel.setText(juce::String::fromUTF8("*  ÂM THANH SẴN SÀNG  |  MẶC ĐỊNH"), juce::dontSendNotification);
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
            groupToggles[i].setButtonText(enabled ? juce::String::fromUTF8("BẬT") : juce::String::fromUTF8("TẮT"));
        };
        addAndMakeVisible(toggle);
    }

    // Left: input and output gain.
    addParameter(juce::String::fromUTF8("Độ lợi đầu vào"), -12.0, 24.0, 0.0, " dB", 0);
    addParameter(juce::String::fromUTF8("Độ lợi đầu ra"), -24.0, 12.0, 0.0, " dB", 0);

    // Parametric EQ: five visual bands. DSP connection is a later stage.
    addParameter(juce::String::fromUTF8("TRẦM  |  80 Hz"), -12.0, 12.0, 3.0, " dB", 1);
    addParameter(juce::String::fromUTF8("TRẦM VỪA  |  250 Hz"), -12.0, 12.0, 1.5, " dB", 1);
    addParameter("TRUNG  |  1,0 kHz", -12.0, 12.0, 0.0, " dB", 1);
    addParameter(juce::String::fromUTF8("CAO VỪA  |  4,0 kHz"), -12.0, 12.0, 2.0, " dB", 1);
    addParameter("CAO  |  12 kHz", -12.0, 12.0, 1.0, " dB", 1);

    addParameter(juce::String::fromUTF8("Ngưỡng"), -60.0, 0.0, -18.0, " dB", 2);
    addParameter(juce::String::fromUTF8("Tỷ lệ nén"), 1.0, 12.0, 3.0, " : 1", 2);
    addParameter(juce::String::fromUTF8("Thời gian tấn công"), 1.0, 100.0, 10.0, " ms", 2);
    addParameter(juce::String::fromUTF8("Thời gian nhả"), 10.0, 500.0, 150.0, " ms", 2);

    addParameter(juce::String::fromUTF8("Ngưỡng"), -60.0, 0.0, -30.0, " dB", 3);
    addParameter(juce::String::fromUTF8("Tần số"), 2000.0, 12000.0, 6000.0, " Hz", 3);

    addParameter(juce::String::fromUTF8("Độ hiện diện"), 0.0, 100.0, 60.0, " %", 4);
    addParameter(juce::String::fromUTF8("Độ ấm"), 0.0, 100.0, 58.0, " %", 4);
    addParameter(juce::String::fromUTF8("Độ thoáng"), 0.0, 100.0, 60.0, " %", 4);

    addParameter(juce::String::fromUTF8("Ngưỡng"), -80.0, 0.0, -50.0, " dB", 5);
    addParameter(juce::String::fromUTF8("Thời gian giữ"), 0.0, 500.0, 200.0, " ms", 5);
    addParameter(juce::String::fromUTF8("Thời gian nhả"), 0.0, 1000.0, 300.0, " ms", 5);

    addParameter(juce::String::fromUTF8("Âm trầm"), -12.0, 12.0, 2.0, " dB", 6);
    addParameter(juce::String::fromUTF8("Âm trung"), -12.0, 12.0, 0.0, " dB", 6);
    addParameter(juce::String::fromUTF8("Âm cao"), -12.0, 12.0, 2.0, " dB", 6);

    addParameter(juce::String::fromUTF8("Tỷ lệ hòa trộn"), 0.0, 100.0, 20.0, " %", 7);
    addParameter(juce::String::fromUTF8("Không gian phòng"), 0.0, 100.0, 50.0, " %", 7);
    addParameter(juce::String::fromUTF8("Thời gian"), 0.1, 5.0, 1.8, " s", 7);

    masterVolumeLabel.setText(juce::String::fromUTF8("ÂM LƯỢNG TỔNG"), juce::dontSendNotification);
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

    presetSelector.addItem(juce::String::fromUTF8("Giọng nam ấm áp"), 1);
    presetSelector.addItem(juce::String::fromUTF8("Giọng nam dày"), 2);
    presetSelector.addItem(juce::String::fromUTF8("Karaoke nhạc nhẹ"), 3);
    presetSelector.addItem(juce::String::fromUTF8("Giọng mượt mà"), 4);
    presetSelector.addItem(juce::String::fromUTF8("Giọng sáng rõ"), 5);
    presetSelector.addItem(juce::String::fromUTF8("Giọng trầm ấm"), 6);
    presetSelector.addItem(juce::String::fromUTF8("Giọng sân khấu"), 7);
    presetSelector.addItem(juce::String::fromUTF8("Giọng truyền cảm"), 8);
    presetSelector.addItem(juce::String::fromUTF8("Karaoke tự nhiên"), 9);
    presetSelector.addItem(juce::String::fromUTF8("MC nam trầm"), 10);
    presetSelector.addItem(juce::String::fromUTF8("MC nữ trung âm"), 11);
    presetSelector.addItem(juce::String::fromUTF8("Nữ cao sáng"), 12);
    presetSelector.addItem(juce::String::fromUTF8("Nữ thanh thoát"), 13);
    presetSelector.addItem(juce::String::fromUTF8("Nữ trữ tình"), 14);
    presetSelector.setSelectedId(1);
    presetSelector.onChange = [this]
    {
        applyBuiltInPreset(presetSelector.getSelectedId());
    };
    applyBuiltInPreset(1);
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
    styleTextButton(pitchCorrectionButton, juce::Colour(18, 91, 145));
    styleTextButton(vocalLevelingButton, juce::Colour(18, 105, 145));
    styleTextButton(vocalLimiterButton, juce::Colour(39, 75, 112));
    styleTextButton(vocalSmoothButton, juce::Colour(70, 83, 120));
    styleTextButton(warmthBodyButton, juce::Colour(105, 67, 39));
    styleTextButton(vocalClarityButton, juce::Colour(39, 92, 91));
    styleTextButton(breathControlButton, juce::Colour(64, 76, 112));
    styleTextButton(vocalExciterButton, juce::Colour(88, 63, 112));
    addAndMakeVisible(savePresetButton);
    addAndMakeVisible(managePresetButton);
    addAndMakeVisible(processButton);
    addAndMakeVisible(bypassButton);
    addAndMakeVisible(resetButton);
    addAndMakeVisible(settingsButton);
    addAndMakeVisible(pitchCorrectionButton);
    addAndMakeVisible(vocalLevelingButton);
    addAndMakeVisible(vocalLimiterButton);
    addAndMakeVisible(vocalSmoothButton);
    addAndMakeVisible(warmthBodyButton);
    addAndMakeVisible(vocalClarityButton);
    addAndMakeVisible(breathControlButton);
    addAndMakeVisible(vocalExciterButton);
    pitchCorrectionButton.onClick = [this] { showPitchCorrectionModule(); };
    vocalLevelingButton.onClick = [this] { showVocalLevelingModule(); };
    vocalLimiterButton.onClick = [this] { showVocalLimiterModule(); };
    vocalSmoothButton.onClick = [this] { showVocalSmoothModule(); };
    warmthBodyButton.onClick = [this] { showWarmthBodyModule(); };
    vocalClarityButton.onClick = [this] { showVocalClarityModule(); };
    breathControlButton.onClick = [this] { showBreathControlModule(); };
    vocalExciterButton.onClick = [this] { showVocalExciterModule(); };

    statusLabel.setText(juce::String::fromUTF8("Sẵn sàng - các bộ xử lý đang ở chế độ giao diện"), juce::dontSendNotification);
    statusLabel.setFont(juce::FontOptions(10.5f));
    statusLabel.setColour(juce::Label::textColourId, muted);
    addAndMakeVisible(statusLabel);

    monitorToggle.onClick = [this]
    {
        const bool enabled = monitorToggle.getToggleState();
        monitorEnabled.store(enabled);
        monitorToggle.setButtonText(enabled ? juce::String::fromUTF8("THEO DÕI ÂM THANH: BẬT") : juce::String::fromUTF8("THEO DÕI ÂM THANH: TẮT"));
        statusLabel.setText(enabled ? juce::String::fromUTF8("Đã bật theo dõi âm thanh trực tiếp")
                                    : juce::String::fromUTF8("Đã tắt theo dõi - không phát âm thanh ra loa"),
                            juce::dontSendNotification);
    };

    processButton.onClick = [this]
    {
        processingEnabled.store(!processingEnabled.load());
        processButton.setButtonText(processingEnabled.load() ? juce::String::fromUTF8("DỪNG XỬ LÝ") : juce::String::fromUTF8("BẬT XỬ LÝ"));
        statusLabel.setText(processingEnabled.load()
            ? juce::String::fromUTF8("Đã bật chuỗi xử lý tín hiệu")
            : juce::String::fromUTF8("Đã tắt xử lý tín hiệu"), juce::dontSendNotification);
    };
    bypassButton.onClick = [this]
    {
        bypassEnabled.store(!bypassEnabled.load());
        statusLabel.setText(bypassEnabled.load() ? juce::String::fromUTF8("BỎ QUA - tín hiệu trực tiếp")
                                                 : juce::String::fromUTF8("Đã tắt chế độ bỏ qua"), juce::dontSendNotification);
    };
    resetButton.onClick = [this]
    {
        for (auto& group : groups)
            for (auto& control : group)
                control->slider.setValue(control->slider.getDoubleClickReturnValue(),
                                         juce::sendNotification);
        masterVolumeSlider.setValue(0.0, juce::sendNotification);
        statusLabel.setText(juce::String::fromUTF8("Đã đưa các giá trị về mặc định"), juce::dontSendNotification);
    };
    savePresetButton.onClick = [this]
    {
        const auto directory = juce::File::getSpecialLocation(
            juce::File::userApplicationDataDirectory)
            .getChildFile("VoiceVocalStudio").getChildFile("Presets");
        directory.createDirectory();
        presetFileChooser = std::make_unique<juce::FileChooser>(
            juce::String::fromUTF8("Lưu cấu hình giọng hát"), directory.getChildFile("CauHinhMoi.vvpreset"),
            "*.vvpreset");
        juce::Component::SafePointer<MainComponent> safeThis(this);
        presetFileChooser->launchAsync(juce::FileBrowserComponent::saveMode
                                       | juce::FileBrowserComponent::canSelectFiles,
            [safeThis](const juce::FileChooser& chooser)
            {
                if (safeThis == nullptr)
                    return;
                auto file = chooser.getResult();
                if (file == juce::File())
                    return;
                if (!file.hasFileExtension("vvpreset"))
                    file = file.withFileExtension("vvpreset");
                safeThis->savePresetToFile(file);
            });
    };
    managePresetButton.onClick = [this]
    {
        const auto directory = juce::File::getSpecialLocation(
            juce::File::userApplicationDataDirectory)
            .getChildFile("VoiceVocalStudio").getChildFile("Presets");
        directory.createDirectory();
        presetFileChooser = std::make_unique<juce::FileChooser>(
            juce::String::fromUTF8("Mở cấu hình đã lưu"), directory, "*.vvpreset");
        juce::Component::SafePointer<MainComponent> safeThis(this);
        presetFileChooser->launchAsync(juce::FileBrowserComponent::openMode
                                       | juce::FileBrowserComponent::canSelectFiles,
            [safeThis](const juce::FileChooser& chooser)
            {
                if (safeThis == nullptr)
                    return;
                const auto file = chooser.getResult();
                if (file != juce::File())
                    safeThis->loadPresetFromFile(file);
            });
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
    for (auto& toggle : groupToggles)
        toggle.setLookAndFeel(nullptr);
    monitorToggle.setLookAndFeel(nullptr);
    shutdownAudio();
}

void MainComponent::applyBuiltInPreset(int presetId)
{
    static const double presetValues[14][8][5] =
    {
        {
            { 0.0, 0.0 },
            { 3.0, 1.5, 0.0, 2.0, 1.0 },
            { -22.0, 2.5, 10.0, 160.0 },
            { -32.0, 6200.0 },
            { 62.0, 60.0, 58.0 },
            { -52.0, 180.0, 280.0 },
            { 2.0, 0.0, 2.0 },
            { 22.0, 48.0, 1.7 }
        },
        {
            { 0.0, 0.0 },
            { 4.0, 2.0, 0.0, 1.5, 0.5 },
            { -24.0, 3.0, 8.0, 140.0 },
            { -30.0, 6000.0 },
            { 64.0, 64.0, 55.0 },
            { -50.0, 220.0, 320.0 },
            { 3.0, 1.0, 1.5 },
            { 18.0, 42.0, 1.5 }
        },
        {
            { 0.0, 0.0 },
            { 2.0, 1.0, 0.0, 2.0, 2.0 },
            { -20.0, 2.0, 15.0, 180.0 },
            { -34.0, 6500.0 },
            { 58.0, 55.0, 65.0 },
            { -55.0, 200.0, 350.0 },
            { 1.5, 0.0, 2.5 },
            { 28.0, 60.0, 2.2 }
        },
        {
            { 0.0, 0.0 },
            { 2.0, 1.0, 0.0, -1.5, -1.0 },
            { -24.0, 2.2, 15.0, 180.0 },
            { -36.0, 5800.0 },
            { 54.0, 58.0, 48.0 },
            { -54.0, 220.0, 320.0 },
            { 1.5, 0.0, -1.0 },
            { 24.0, 52.0, 1.9 }
        },
        {
            { 0.0, 0.0 },
            { 1.0, 0.0, 0.0, 2.5, 2.0 },
            { -23.0, 2.5, 8.0, 130.0 },
            { -32.0, 6500.0 },
            { 68.0, 52.0, 68.0 },
            { -50.0, 180.0, 260.0 },
            { 0.0, 1.0, 2.0 },
            { 16.0, 40.0, 1.3 }
        },
        {
            { 0.0, 0.0 },
            { 4.0, 2.0, 0.0, -0.5, -1.0 },
            { -23.0, 2.8, 12.0, 170.0 },
            { -32.0, 6000.0 },
            { 55.0, 68.0, 48.0 },
            { -52.0, 200.0, 300.0 },
            { 3.0, 0.0, -1.0 },
            { 20.0, 48.0, 1.6 }
        },
        {
            { 0.0, 0.0 },
            { 3.0, 1.0, 0.0, 2.0, 1.5 },
            { -26.0, 3.0, 8.0, 150.0 },
            { -33.0, 6200.0 },
            { 68.0, 58.0, 62.0 },
            { -50.0, 180.0, 280.0 },
            { 2.0, 0.0, 2.0 },
            { 30.0, 65.0, 2.4 }
        },
        {
            { 0.0, 0.0 },
            { 2.0, 1.0, 0.0, 1.0, 0.5 },
            { -25.0, 2.4, 18.0, 220.0 },
            { -35.0, 6000.0 },
            { 60.0, 62.0, 54.0 },
            { -55.0, 250.0, 380.0 },
            { 2.0, 1.0, 0.0 },
            { 28.0, 58.0, 2.2 }
        },
        {
            { 0.0, 0.0 },
            { 2.0, 1.0, 0.0, 1.0, 1.0 },
            { -21.0, 2.0, 15.0, 180.0 },
            { -34.0, 6200.0 },
            { 58.0, 56.0, 58.0 },
            { -56.0, 220.0, 350.0 },
            { 1.0, 0.0, 1.0 },
            { 20.0, 50.0, 1.8 }
        },
        {
            { 0.0, 0.0 },
            { 2.5, 1.5, -0.5, 1.0, 0.5 },
            { -24.0, 2.8, 12.0, 180.0 },
            { -32.0, 6200.0 },
            { 56.0, 62.0, 50.0 },
            { -54.0, 180.0, 300.0 },
            { 2.0, 0.5, 1.0 },
            { 15.0, 38.0, 1.4 }
        },
        {
            { 0.0, 0.0 },
            { 0.5, 0.5, 0.5, 1.5, 1.0 },
            { -26.0, 2.5, 8.0, 160.0 },
            { -34.0, 6800.0 },
            { 58.0, 48.0, 58.0 },
            { -56.0, 160.0, 280.0 },
            { 0.0, 0.5, 1.5 },
            { 14.0, 36.0, 1.3 }
        },
        {
            { 0.0, 0.0 },
            { -1.0, 0.0, 0.5, 1.0, 1.5 },
            { -28.0, 2.2, 6.0, 140.0 },
            { -36.0, 7200.0 },
            { 54.0, 45.0, 62.0 },
            { -58.0, 140.0, 250.0 },
            { -0.5, 0.0, 1.0 },
            { 12.0, 34.0, 1.2 }
        },
        {
            { 0.0, 0.0 },
            { 0.0, 0.0, 0.0, 1.5, 2.0 },
            { -27.0, 2.5, 8.0, 160.0 },
            { -36.0, 7000.0 },
            { 60.0, 48.0, 70.0 },
            { -56.0, 150.0, 260.0 },
            { 0.0, 0.0, 1.5 },
            { 18.0, 42.0, 1.5 }
        },
        {
            { 0.0, 0.0 },
            { 0.5, 1.0, 0.0, 1.0, 1.0 },
            { -25.0, 2.6, 10.0, 190.0 },
            { -34.0, 6500.0 },
            { 58.0, 56.0, 62.0 },
            { -54.0, 180.0, 300.0 },
            { 0.5, 0.5, 1.0 },
            { 24.0, 52.0, 1.8 }
        }
    };
    static const int parameterCounts[8] = { 2, 5, 4, 2, 3, 3, 3, 3 };
    static const juce::String presetNames[14] =
    {
        juce::String::fromUTF8("Giọng nam ấm áp"), juce::String::fromUTF8("Giọng nam dày"), juce::String::fromUTF8("Karaoke nhạc nhẹ"),
        juce::String::fromUTF8("Giọng mượt mà"), juce::String::fromUTF8("Giọng sáng rõ"), juce::String::fromUTF8("Giọng trầm ấm"),
        juce::String::fromUTF8("Giọng sân khấu"), juce::String::fromUTF8("Giọng truyền cảm"), juce::String::fromUTF8("Karaoke tự nhiên"),
        juce::String::fromUTF8("MC nam trầm"), juce::String::fromUTF8("MC nữ trung âm"), juce::String::fromUTF8("Nữ cao sáng"),
        juce::String::fromUTF8("Nữ thanh thoát"), juce::String::fromUTF8("Nữ trữ tình")
    };

    if (presetId < 1 || presetId > 14)
        return;

    const int presetIndex = presetId - 1;
    for (int group = 0; group < 8; ++group)
    {
        for (int parameter = 0; parameter < parameterCounts[group]; ++parameter)
        {
            auto& slider = groups[(size_t)group][(size_t)parameter]->slider;
            slider.setValue(presetValues[presetIndex][group][parameter],
                            juce::sendNotification);
        }

        groupToggles[(size_t)group].setToggleState(true, juce::dontSendNotification);
        groupEnabled[(size_t)group].store(true);
        groupToggles[(size_t)group].setButtonText(juce::String::fromUTF8("BẬT"));
    }

    masterVolumeSlider.setValue(0.0, juce::sendNotification);
    statusLabel.setText(juce::String::fromUTF8("Đã nạp cấu hình: ") + presetNames[presetIndex],
                        juce::dontSendNotification);
}

void MainComponent::savePresetToFile(const juce::File& file)
{
    auto preset = std::make_unique<juce::XmlElement>("VOICE_VOCAL_PRESET");
    preset->setAttribute("version", 1);
    preset->setAttribute("masterVolumeDb", masterVolumeSlider.getValue());

    for (size_t group = 0; group < groups.size(); ++group)
    {
        preset->setAttribute("group" + juce::String((int)group) + "Enabled",
                             groupToggles[group].getToggleState());
        for (size_t parameter = 0; parameter < groups[group].size(); ++parameter)
        {
            const auto key = "g" + juce::String((int)group)
                + "p" + juce::String((int)parameter);
            preset->setAttribute(key, groups[group][parameter]->slider.getValue());
        }
    }

    if (preset->writeTo(file))
        statusLabel.setText(juce::String::fromUTF8("Đã lưu cấu hình: ") + file.getFileNameWithoutExtension(),
                            juce::dontSendNotification);
    else
        statusLabel.setText(juce::String::fromUTF8("Không thể lưu tệp cấu hình"), juce::dontSendNotification);
}

bool MainComponent::loadPresetFromFile(const juce::File& file)
{
    auto xml = juce::XmlDocument::parse(file);
    if (xml == nullptr || !xml->hasTagName("VOICE_VOCAL_PRESET"))
    {
        statusLabel.setText(juce::String::fromUTF8("Tệp cấu hình không hợp lệ"), juce::dontSendNotification);
        return false;
    }

    for (size_t group = 0; group < groups.size(); ++group)
    {
        const auto enabledKey = "group" + juce::String((int)group) + "Enabled";
        const bool enabled = xml->getBoolAttribute(enabledKey, true);
        groupToggles[group].setToggleState(enabled, juce::dontSendNotification);
        groupEnabled[group].store(enabled);
        groupToggles[group].setButtonText(enabled ? juce::String::fromUTF8("BẬT") : juce::String::fromUTF8("TẮT"));

        for (size_t parameter = 0; parameter < groups[group].size(); ++parameter)
        {
            const auto key = "g" + juce::String((int)group)
                + "p" + juce::String((int)parameter);
            if (xml->hasAttribute(key))
                groups[group][parameter]->slider.setValue(
                    xml->getDoubleAttribute(key), juce::sendNotification);
        }
    }

    masterVolumeSlider.setValue(xml->getDoubleAttribute("masterVolumeDb", 0.0),
                                juce::sendNotification);
    statusLabel.setText(juce::String::fromUTF8("Đã nạp cấu hình: ") + file.getFileNameWithoutExtension(),
                        juce::dontSendNotification);
    return true;
}

void MainComponent::styleToggle(juce::ToggleButton& toggle)
{
    toggle.setLookAndFeel(&toggleLookAndFeel);
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
        if (name == juce::String::fromUTF8("Độ lợi đầu vào"))
            inputGainLinear.store(juce::Decibels::decibelsToGain(value));
        else if (name == juce::String::fromUTF8("Độ lợi đầu ra"))
            outputGainLinear.store(juce::Decibels::decibelsToGain(value));
        else if (group == 1 && parameterIndex >= 0
                 && parameterIndex < (int)eqGainDb.size())
            eqGainDb[(size_t)parameterIndex].store(value);
        else if (group == 2)
        {
            if (parameterIndex == 0)
                compressorThresholdDb.store(value);
            else if (parameterIndex == 1)
                compressorRatio.store(value);
            else if (parameterIndex == 2)
                compressorAttackMs.store(value);
            else if (parameterIndex == 3)
                compressorReleaseMs.store(value);
        }
        else if (group == 3)
        {
            if (parameterIndex == 0)
                deEsserThresholdDb.store(value);
            else if (parameterIndex == 1)
                deEsserFrequencyHz.store(value);
        }
        else if (group == 4)
        {
            if (parameterIndex == 0)
                voicePresence.store(value);
            else if (parameterIndex == 1)
                voiceWarmth.store(value);
            else if (parameterIndex == 2)
                voiceAir.store(value);
        }
        else if (group == 5)
        {
            if (parameterIndex == 0)
                noiseGateThresholdDb.store(value);
            else if (parameterIndex == 1)
                noiseGateHoldMs.store(value);
            else if (parameterIndex == 2)
                noiseGateReleaseMs.store(value);
        }
        else if (group == 6)
        {
            if (parameterIndex == 0)
                toneBassDb.store(value);
            else if (parameterIndex == 1)
                toneMidDb.store(value);
            else if (parameterIndex == 2)
                toneTrebleDb.store(value);
        }
        else if (group == 7)
        {
            if (parameterIndex == 0)
                reverbMixPercent.store(value);
            else if (parameterIndex == 1)
                reverbRoomPercent.store(value);
            else if (parameterIndex == 2)
                reverbTimeSeconds.store(value);
        }
    };

    groups[(size_t)group].push_back(std::move(control));
    return result;
}

MainComponent::PanelBounds MainComponent::calculatePanelBounds() const
{
    PanelBounds p;
    const int w = getWidth();
    const int h = getHeight();
    const int margin = juce::jlimit(12, 15, w / 100);
    const int gap = juce::jlimit(9, 11, w / 130);
    const int headerBottom = juce::jlimit(82, 88, h / 9);
    const int footerHeight = juce::jlimit(70, 76, h / 10);
    const int moduleRowHeight = 42;
    const int availablePanelsHeight = h - headerBottom - footerHeight
                                    - moduleRowHeight - margin * 2 - gap * 2;
    const int bottomHeight = juce::jlimit(176, 194,
                                          (int)(availablePanelsHeight * 0.33f));
    const int bottomY = h - footerHeight - moduleRowHeight - bottomHeight - margin;
    const int topHeight = juce::jmax(250, bottomY - headerBottom - gap);
    const int leftWidth = juce::jlimit(210, 238, (int)(w * 0.18f));
    const int rightWidth = juce::jlimit(270, 300, (int)(w * 0.235f));
    const int centreWidth = w - margin * 2 - gap * 2 - leftWidth - rightWidth;
    const int rightX = margin + leftWidth + gap + centreWidth + gap;

    p[0] = { margin, headerBottom, leftWidth, topHeight };
    p[1] = { margin + leftWidth + gap, headerBottom, centreWidth, topHeight };
    const int rightTopHeight = (topHeight - gap) * 70 / 100;
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
    const float headerHeight = (float)juce::jlimit(82, 88, getHeight() / 9);
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
    drawPanel(g, panels[0], juce::String::fromUTF8("ĐẦU VÀO / ĐẦU RA"));
    drawPanel(g, panels[1], juce::String::fromUTF8("BỘ LỌC EQ THAM SỐ"));
    drawPanel(g, panels[2], juce::String::fromUTF8("NÉN ÂM THANH"));
    drawPanel(g, panels[3], juce::String::fromUTF8("GIẢM ÂM XÌ"));
    drawPanel(g, panels[4], juce::String::fromUTF8("CẢI THIỆN GIỌNG HÁT"), juce::Colour(255, 190, 65));
    drawPanel(g, panels[5], juce::String::fromUTF8("CỔNG CHẶN TIẾNG ỒN"));
    drawPanel(g, panels[6], juce::String::fromUTF8("ĐIỀU CHỈNH ÂM SẮC"));
    drawPanel(g, panels[7], juce::String::fromUTF8("TẠO VANG"), juce::Colour(90, 190, 255));

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

    g.setFont(juce::FontOptions(9.5f, juce::Font::bold));
    g.setColour(muted);
    const juce::String meterScale[] { "0", "-6", "-12", "-24", "-48" };
    for (int meterIndex = 0; meterIndex < 5; ++meterIndex)
    {
        const float y1 = (float)inputPanel.getY() + 72.0f
            + (float)inputPanel.getHeight() * 0.34f * (float)meterIndex / 5.0f;
        const float y2 = (float)inputPanel.getY()
            + inputPanel.getHeight() * 0.53f
            + (float)inputPanel.getHeight() * 0.34f * (float)meterIndex / 5.0f;
        g.drawText(meterScale[meterIndex],
                   inputPanel.getRight() - 18, (int)y1 - 5, 18, 12,
                   juce::Justification::centredLeft);
        g.drawText(meterScale[meterIndex],
                   inputPanel.getRight() - 18, (int)y2 - 5, 18, 12,
                   juce::Justification::centredLeft);
    }

    // Live compressor gain-reduction meter (0 to 18 dB displayed).
    const float gainReductionLevel = juce::jlimit(0.0f, 1.0f,
        compressorGainReductionDb.load() / 18.0f);
    drawLedMeter({ (float)panels[2].getRight() - 34.0f,
                   (float)panels[2].getY() + 72.0f, 22.0f,
                   (float)panels[2].getHeight() - 96.0f },
                 gainReductionLevel, true);

    // EQ response graph, with five editable-band markers as a visual guide.
    auto eq = panels[1].reduced(14);
    auto graph = juce::Rectangle<float>((float)eq.getX() + 8.0f,
        (float)eq.getY() + 52.0f, (float)eq.getWidth() - 16.0f,
        (float)eq.getHeight() * 0.36f);
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

    // Subtle decorative spectrum texture behind the EQ curve.
    // It is intentionally visual-only; the displayed response curve remains tied to EQ controls.
    juce::Path spectrum;
    spectrum.startNewSubPath(graph.getX(), graph.getBottom() - 2.0f);
    constexpr int spectrumBars = 120;
    for (int i = 0; i < spectrumBars; ++i)
    {
        const float t = (float)i / (float)(spectrumBars - 1);
        const float envelope = 0.20f
            + 0.34f * std::exp(-std::pow((t - 0.52f) / 0.23f, 2.0f))
            + 0.16f * std::exp(-std::pow((t - 0.82f) / 0.10f, 2.0f));
        const float ripple = 0.35f + 0.65f
            * std::abs(std::sin((float)i * 1.73f) * std::cos((float)i * 0.37f));
        const float barHeight = graph.getHeight() * envelope * ripple * 0.48f;
        const float x = graph.getX() + t * graph.getWidth();
        g.setColour(blue.withAlpha(0.10f + 0.12f * ripple));
        g.drawLine(x, graph.getBottom() - 2.0f,
                   x, graph.getBottom() - 2.0f - barHeight, 1.0f);
        spectrum.lineTo(x, graph.getBottom() - 2.0f - barHeight);
    }
    spectrum.lineTo(graph.getRight(), graph.getBottom() - 2.0f);
    g.setColour(blue.withAlpha(0.08f));
    g.strokePath(spectrum, juce::PathStrokeType(1.0f));

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

    // Tần số scale below the response graph.
    g.setFont(juce::FontOptions(10.0f));
    g.setColour(muted);
    const juce::String freqLabels[] { "20", "50", "100", "200", "500", "1k", "2k", "5k", "10k", "20k" };
    const float labelFrequencies[] { 20.0f, 50.0f, 100.0f, 200.0f, 500.0f,
                                      1000.0f, 2000.0f, 5000.0f, 10000.0f, 20000.0f };
    for (int i = 0; i < 10; ++i)
    {
        const float x = frequencyToX(labelFrequencies[i]);
        g.drawText(freqLabels[i], (int)x - 18, (int)graph.getBottom() + 3, 36, 15,
                   juce::Justification::centred);
    }

    // Footer plate.
    const float footerHeight = (float)juce::jlimit(70, 76, getHeight() / 10);
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
    const int headerH = juce::jlimit(82, 88, h / 9);
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
        const int availableWidth = juce::jmax(40, a.getWidth() - 72);
        const int inputControlCount = juce::jmin(2, (int)groups[0].size());
        const int firstRowY = a.getY() + 48;
        const int monitorTop = a.getBottom() - 38;
        const int rowAreaHeight = juce::jmax(80, monitorTop - firstRowY - 10);
        const int rowHeight = rowAreaHeight / juce::jmax(1, inputControlCount);
        const int labelHeight = 22;
        for (int i = 0; i < inputControlCount; ++i)
        {
            auto& c = *groups[0][(size_t)i];
            const int rowY = firstRowY + i * rowHeight;
            const int sliderHeight = juce::jmax(24, rowHeight - labelHeight - 4);
            const int sliderSize = juce::jmin(availableWidth, sliderHeight);
            c.label.setBounds(a.getX(), rowY, a.getWidth() - 58, labelHeight);
            c.slider.setBounds(a.getX() + (availableWidth - sliderSize) / 2,
                               rowY + labelHeight, sliderSize, sliderHeight);
        }
        monitorToggle.setBounds(a.getX() + 8, a.getBottom() - 34, a.getWidth() - 16, 28);
    }

    // EQ: five compact knobs in a single row under the response curve.
    {
        auto a = panels[1].reduced(12);
        const int top = a.getY() + (int)(a.getHeight() * 0.64f);
        const int slot = a.getWidth() / 5;
        const int eqControlCount = juce::jmin(5, (int)groups[1].size());
        const int availableHeight = juce::jmax(30, a.getBottom() - top - 4);
        const int sliderSize = juce::jmin(juce::jmin(124, slot - 8), availableHeight - 24);
        for (int i = 0; i < eqControlCount; ++i)
        {
            auto& c = *groups[1][(size_t)i];
            const int x = a.getX() + i * slot;
            c.label.setBounds(x, top, slot, 22);
            c.slider.setBounds(x + (slot - sliderSize) / 2, top + 22,
                               sliderSize, sliderSize + 20);
        }
    }

    auto layoutSmallGroup = [this](int group, juce::Rectangle<int> panel, int columns)
    {
        auto area = panel.reduced(9);
        const int count = (int)groups[(size_t)group].size();
        if (count <= 0 || columns <= 0)
            return;

        const int rows = (count + columns - 1) / columns;
        const int reservedRight = group == 2 ? 30 : 0;
        area.setRight(area.getRight() - reservedRight);

        // Each control gets a strictly bounded cell. The rotary slider is
        // square and is never allowed to extend into the next row/column.
        const int contentTop = area.getY() + 43;
        const int contentBottom = area.getBottom() - 5;
        const int contentHeight = juce::jmax(24, contentBottom - contentTop);
        const int slotW = area.getWidth() / columns;
        const int slotH = contentHeight / rows;
        const int labelHeight = 20;

        for (int i = 0; i < count; ++i)
        {
            auto& c = *groups[(size_t)group][(size_t)i];
            const int col = i % columns;
            const int row = i / columns;
            const int cellX = area.getX() + col * slotW;
            const int cellY = contentTop + row * slotH;
            const int sliderAvailableHeight = juce::jmax(22, slotH - labelHeight - 2);
            const int sliderSize = juce::jmax(22,
                juce::jmin(juce::jmin(104, slotW - 6), sliderAvailableHeight - 20));

            c.label.setBounds(cellX, cellY, slotW, labelHeight);
            c.slider.setBounds(cellX + (slotW - sliderSize) / 2,
                               cellY + labelHeight,
                               sliderSize, sliderSize + 20);
        }
    };

    // Compressor uses a balanced 2 x 2 layout so labels and controls have room.
    layoutSmallGroup(2, panels[2], 2);
    layoutSmallGroup(3, panels[3], 2);
    for (int group = 4; group < 8; ++group)
        layoutSmallGroup(group, panels[(size_t)group], 3);

    const int footerTop = getHeight() - juce::jlimit(70, 76, getHeight() / 10);
    const int moduleRowTop = footerTop - 42;
    const int moduleMargin = 14;
    const int moduleGap = 8;
    const int moduleButtonWidth = (w - moduleMargin * 2 - moduleGap * 7) / 8;
    juce::TextButton* moduleButtons[] {
        &pitchCorrectionButton, &vocalLevelingButton, &vocalLimiterButton,
        &warmthBodyButton, &vocalClarityButton, &breathControlButton,
        &vocalExciterButton, &vocalSmoothButton
    };
    for (int i = 0; i < 8; ++i)
        moduleButtons[i]->setBounds(moduleMargin + i * (moduleButtonWidth + moduleGap),
                                    moduleRowTop + 5, moduleButtonWidth, 32);

    const int footerY = footerTop + 8;
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
    statusLabel.setBounds(18, footerTop + 48, w - 36, 18);
}

void MainComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    juce::ignoreUnused(samplesPerBlockExpected);
    currentSampleRate = sampleRate > 0.0 ? sampleRate : 44100.0;
    pitchAnalysisBuffer.fill(0.0f);
    pitchAnalysisWritePosition = 0;
    pitchAnalysisSamplesSinceUpdate = 0;
    pitchDetectedMidi.store(-1);
    pitchTargetMidi.store(-1);
    pitchDeviationCents.store(0.0f);
    pitchShifter.reset();
    vocalLevelEnvelope = 0.0f;
    vocalLevelGain = 1.0f;
    vocalLimiterGain = 1.0f;
    for (auto& filter : eqFilters)
        filter.reset();
    lastAppliedEqGainDb.fill(999.0f);
    compressorEnvelope = 0.0f;
    compressorGainReductionDb.store(0.0f);
    deEsserEnvelope = 0.0f;
    lastDeEsserFrequencyHz = 0.0f;
    deEsserDetectorFilter.reset();
    deEsserBandFilter.reset();
    for (auto& filter : voiceEnhanceFilters)
        filter.reset();
    lastVoiceEnhanceGainDb.fill(999.0f);
    warmthFilter.reset();
    bodyFilter.reset();
    lastWarmthGainDb = 999.0f;
    lastBodyGainDb = 999.0f;
    clarityFilter.reset();
    clarityAirFilter.reset();
    const auto smoothCoefficients = makeBandPassCoefficients((float)currentSampleRate, 3400.0f, 1.05f);
    vocalSmoothDetectorFilter.setCoefficients(smoothCoefficients[0], smoothCoefficients[1],
        smoothCoefficients[2], smoothCoefficients[3], smoothCoefficients[4]);
    vocalSmoothBandFilter.setCoefficients(smoothCoefficients[0], smoothCoefficients[1],
        smoothCoefficients[2], smoothCoefficients[3], smoothCoefficients[4]);
    vocalSmoothDetectorFilter.reset();
    vocalSmoothBandFilter.reset();
    vocalSmoothEnvelope = 0.0f;
    vocalSmoothGain = 1.0f;
    const auto exciterCoefficients = makeBandPassCoefficients((float)currentSampleRate, 4800.0f, 0.55f);
    vocalExciterFilter.setCoefficients(exciterCoefficients[0], exciterCoefficients[1],
        exciterCoefficients[2], exciterCoefficients[3], exciterCoefficients[4]);
    vocalExciterFilter.reset();
    vocalExciterMixGain = 0.0f;
    const auto breathCoefficients = makeBandPassCoefficients((float)currentSampleRate, 5500.0f, 0.70f);
    breathDetectorFilter.setCoefficients(breathCoefficients[0], breathCoefficients[1],
        breathCoefficients[2], breathCoefficients[3], breathCoefficients[4]);
    breathBandFilter.setCoefficients(breathCoefficients[0], breathCoefficients[1],
        breathCoefficients[2], breathCoefficients[3], breathCoefficients[4]);
    breathDetectorEnvelope = 0.0f;
    breathVoiceEnvelope = 0.0f;
    breathReductionGain = 1.0f;
    lastClarityGainDb = 999.0f;
    lastClarityAirGainDb = 999.0f;
    noiseGateEnvelope = 0.0f;
    noiseGateGain = 1.0f;
    noiseGateHoldCounter = 0;
    for (auto& filter : toneFilters)
        filter.reset();
    lastToneGainDb.fill(999.0f);
    reverb.reset();
    lastReverbMixPercent = -1.0f;
    lastReverbRoomPercent = -1.0f;
    lastReverbTimeSeconds = -1.0f;
    reverbWasActive = false;
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

    // Lightweight monophonic pitch tracker. Analysis runs periodically on a
    // fixed-size ring buffer; no allocation or locking occurs in the callback.
    for (int i = 0; i < info.numSamples; ++i)
    {
        pitchAnalysisBuffer[(size_t)pitchAnalysisWritePosition] = input[i];
        pitchAnalysisWritePosition = (pitchAnalysisWritePosition + 1) % 2048;
        ++pitchAnalysisSamplesSinceUpdate;
    }

    if (pitchAnalysisSamplesSinceUpdate >= 1024)
    {
        pitchAnalysisSamplesSinceUpdate = 0;
        const float analysisSampleRate = (float)currentSampleRate;
        float mean = 0.0f;
        for (int i = 0; i < 1024; ++i)
        {
            const int index = (pitchAnalysisWritePosition + 1024 + i) % 2048;
            mean += pitchAnalysisBuffer[(size_t)index];
        }
        mean *= (1.0f / 1024.0f);

        float bestCorrelation = 0.0f;
        int bestLag = 0;
        const int minLag = juce::jmax(24, (int)(analysisSampleRate / 1000.0f));
        const int maxLag = juce::jmin(700, (int)(analysisSampleRate / 65.0f));
        for (int lag = minLag; lag <= maxLag; ++lag)
        {
            float cross = 0.0f, energyA = 0.0f, energyB = 0.0f;
            for (int n = 0; n < 1024 - lag; n += 2)
            {
                const float a = pitchAnalysisBuffer[(size_t)((pitchAnalysisWritePosition + 1024 + n) % 2048)] - mean;
                const float b = pitchAnalysisBuffer[(size_t)((pitchAnalysisWritePosition + 1024 + n + lag) % 2048)] - mean;
                cross += a * b;
                energyA += a * a;
                energyB += b * b;
            }
            const float denominator = std::sqrt(energyA * energyB) + 1.0e-12f;
            const float correlation = cross / denominator;
            if (correlation > bestCorrelation)
            {
                bestCorrelation = correlation;
                bestLag = lag;
            }
        }

        if (bestLag > 0 && bestCorrelation > 0.72f)
        {
            const float frequency = analysisSampleRate / (float)bestLag;
            const int midi = juce::roundToInt(69.0f + 12.0f * std::log2(frequency / 440.0f));
            const int root = pitchSongKey.load();
            const int scale = pitchScale.load();
            const int major[] { 0, 2, 4, 5, 7, 9, 11 };
            const int minor[] { 0, 2, 3, 5, 7, 8, 10 };
            int nearest = midi;
            int bestDistance = 127;
            for (int candidate = midi - 6; candidate <= midi + 6; ++candidate)
            {
                const int pitchClass = (candidate % 12 + 12) % 12;
                const int relative = (pitchClass - root + 12) % 12;
                bool allowed = scale == 3;
                if (scale == 1)
                    for (int note : major) allowed = allowed || relative == note;
                else if (scale == 2)
                    for (int note : minor) allowed = allowed || relative == note;
                if (allowed && std::abs(candidate - midi) < bestDistance)
                {
                    bestDistance = std::abs(candidate - midi);
                    nearest = candidate;
                }
            }
            const float targetFrequency = 440.0f * std::pow(2.0f, (nearest - 69) / 12.0f);
            const float cents = 1200.0f * std::log2(frequency / targetFrequency);
            pitchDetectedMidi.store(midi);
            pitchTargetMidi.store(nearest);
            pitchDeviationCents.store(cents);
        }
        else
        {
            pitchDetectedMidi.store(-1);
            pitchTargetMidi.store(-1);
            pitchDeviationCents.store(0.0f);
        }
    }

    // Processing and bypass control the complete DSP chain. Master Volume
    // remains available even while the processing chain is bypassed.
    const bool useGainStage = processingEnabled.load() && !bypassEnabled.load();
    const float sampleRate = (float)currentSampleRate;
    const bool eqActive = useGainStage && groupEnabled[1].load();
    const bool compressorActive = useGainStage && groupEnabled[2].load();
    const bool deEsserActive = useGainStage && groupEnabled[3].load();
    const bool voiceEnhanceActive = useGainStage && groupEnabled[4].load();
    const bool warmthBodyActive = useGainStage && warmthBodyEnabled.load();
    const bool vocalClarityActive = useGainStage && vocalClarityEnabled.load();
    const bool vocalSmoothActive = useGainStage && vocalSmoothEnabled.load();
    const bool breathControlActive = useGainStage && breathControlEnabled.load();
    const bool vocalExciterActive = useGainStage && vocalExciterEnabled.load();
    const float exciterAmountValue = juce::jlimit(0.0f, 100.0f, vocalExciterAmount.load());
    const float exciterMixValue = juce::jlimit(0.0f, 100.0f, vocalExciterMix.load());
    const float exciterDrive = 1.0f + exciterAmountValue * 0.055f;
    const float exciterNormaliser = std::tanh(exciterDrive);
    const float exciterTargetMix = vocalExciterActive ? exciterMixValue * 0.0035f : 0.0f;
    const float exciterMixSmoothing = std::exp(-1.0f / (sampleRate * 0.025f));
    const float smoothDetectorAttack = std::exp(-1.0f / (sampleRate * 0.006f));
    const float smoothDetectorRelease = std::exp(-1.0f / (sampleRate * 0.110f));
    const float smoothGainAttack = std::exp(-1.0f / (sampleRate * 0.008f));
    const float smoothGainRelease = std::exp(-1.0f / (sampleRate * 0.100f));
    const float smoothSensitivityValue = juce::jlimit(0.0f, 100.0f,
                                                       vocalSmoothSensitivity.load());
    const float smoothIntensityValue = juce::jlimit(0.0f, 100.0f,
                                                     vocalSmoothIntensity.load());
    const float smoothThresholdDb = -18.0f - smoothSensitivityValue * 0.14f;
    const float smoothMaximumReductionDb = smoothIntensityValue * 0.055f;
    const bool noiseGateActive = useGainStage && groupEnabled[5].load();
    const bool toneActive = useGainStage && groupEnabled[6].load();
    const bool reverbActive = useGainStage && groupEnabled[7].load();
    const float channelGain = useGainStage
        ? inputGainLinear.load() * outputGainLinear.load()
        : 1.0f;
    const float masterGain = masterVolumeLinear.load();
    const bool pitchCorrectionActive = useGainStage && pitchCorrectionEnabled.load();
    const int currentPitchTarget = pitchTargetMidi.load();
    const float correctionAmount = juce::jlimit(0.0f, 100.0f,
                                                 pitchCorrectionAmount.load());
    const float detectedOffsetCents = pitchDeviationCents.load();
    const float appliedCorrectionCents = pitchCorrectionActive && currentPitchTarget >= 0
        ? juce::jlimit(-100.0f, 100.0f, detectedOffsetCents) * correctionAmount / 100.0f
        : 0.0f;
    const float pitchTargetRatio = std::pow(2.0f, -appliedCorrectionCents / 1200.0f);
    const float pitchSmoothingMs = 125.0f
        - juce::jlimit(5.0f, 100.0f, pitchCorrectionSpeed.load()) * 1.10f;
    const bool vocalLevelingActive = useGainStage && vocalLevelingEnabled.load();
    const float vocalTargetLinear = juce::Decibels::decibelsToGain(
        juce::jlimit(-30.0f, -12.0f, vocalLevelTargetDb.load()));
    const float vocalMaximumBoost = juce::Decibels::decibelsToGain(
        juce::jlimit(0.0f, 12.0f, vocalLevelMaxBoostDb.load()));
    const float vocalMinimumGain = juce::Decibels::decibelsToGain(
        -juce::jlimit(0.0f, 12.0f, vocalLevelMaxCutDb.load()));
    const float vocalEnvelopeAttack = std::exp(-1.0f / (sampleRate * 0.030f));
    const float vocalEnvelopeRelease = std::exp(-1.0f / (sampleRate * 0.250f));
    const float vocalGainUp = std::exp(-1.0f / (sampleRate * 0.120f));
    const float vocalGainDown = std::exp(-1.0f / (sampleRate * 0.350f));
    const float vocalGainReturn = std::exp(-1.0f / (sampleRate * 0.080f));

    if (eqActive)
    {
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

    const float thresholdDb = compressorThresholdDb.load();
    const float ratio = juce::jmax(1.0f, compressorRatio.load());
    const float attackMs = juce::jmax(0.1f, compressorAttackMs.load());
    const float releaseMs = juce::jmax(1.0f, compressorReleaseMs.load());
    const float attackCoefficient = std::exp(-1.0f / (sampleRate * attackMs * 0.001f));
    const float releaseCoefficient = std::exp(-1.0f / (sampleRate * releaseMs * 0.001f));
    float blockGainReductionDb = 0.0f;

    if (!compressorActive)
        compressorEnvelope = 0.0f;

    const float deEsserThreshold = deEsserThresholdDb.load();
    const float deEsserFrequency = deEsserFrequencyHz.load();
    const float deEsserAttackCoefficient = std::exp(-1.0f / (sampleRate * 0.001f));
    const float deEsserReleaseCoefficient = std::exp(-1.0f / (sampleRate * 0.060f));
    if (deEsserActive && std::abs(deEsserFrequency - lastDeEsserFrequencyHz) > 1.0f)
    {
        const auto coefficients = makeBandPassCoefficients(sampleRate, deEsserFrequency, 2.5f);
        deEsserDetectorFilter.setCoefficients(coefficients[0], coefficients[1],
            coefficients[2], coefficients[3], coefficients[4]);
        deEsserBandFilter.setCoefficients(coefficients[0], coefficients[1],
            coefficients[2], coefficients[3], coefficients[4]);
        deEsserDetectorFilter.reset();
        deEsserBandFilter.reset();
        lastDeEsserFrequencyHz = deEsserFrequency;
    }
    if (!deEsserActive)
    {
        deEsserEnvelope = 0.0f;
        deEsserDetectorFilter.reset();
        deEsserBandFilter.reset();
    }

    if (voiceEnhanceActive)
    {
        const float presenceDb = (voicePresence.load() - 50.0f) * 0.16f;
        const float warmthDb = (voiceWarmth.load() - 50.0f) * 0.16f;
        const float airDb = (voiceAir.load() - 50.0f) * 0.16f;
        const float gains[] { warmthDb, presenceDb, airDb };
        for (int i = 0; i < 3; ++i)
        {
            if (std::abs(gains[i] - lastVoiceEnhanceGainDb[(size_t)i]) > 0.001f)
            {
                BiquadCoefficients coefficients;
                if (i == 0)
                    coefficients = makeLowShelfCoefficients(sampleRate, 150.0f, gains[i]);
                else if (i == 1)
                    coefficients = makePeakingCoefficients(sampleRate, 2800.0f, gains[i], 0.9f);
                else
                    coefficients = makeHighShelfCoefficients(sampleRate, 9500.0f, gains[i]);
                voiceEnhanceFilters[(size_t)i].setCoefficients(coefficients[0], coefficients[1],
                    coefficients[2], coefficients[3], coefficients[4]);
                lastVoiceEnhanceGainDb[(size_t)i] = gains[i];
            }
        }
    }
    else
    {
        for (auto& filter : voiceEnhanceFilters)
            filter.reset();
    }

    if (warmthBodyActive)
    {
        const float warmthDb = (juce::jlimit(0.0f, 100.0f, warmthAmount.load()) - 50.0f) * 0.08f;
        const float bodyDb = (juce::jlimit(0.0f, 100.0f, bodyAmount.load()) - 50.0f) * 0.06f;
        if (std::abs(warmthDb - lastWarmthGainDb) > 0.001f)
        {
            const auto coefficients = makeLowShelfCoefficients(sampleRate, 145.0f, warmthDb);
            warmthFilter.setCoefficients(coefficients[0], coefficients[1],
                coefficients[2], coefficients[3], coefficients[4]);
            lastWarmthGainDb = warmthDb;
        }
        if (std::abs(bodyDb - lastBodyGainDb) > 0.001f)
        {
            const auto coefficients = makePeakingCoefficients(sampleRate, 230.0f, bodyDb, 0.85f);
            bodyFilter.setCoefficients(coefficients[0], coefficients[1],
                coefficients[2], coefficients[3], coefficients[4]);
            lastBodyGainDb = bodyDb;
        }
    }
    else
    {
        warmthFilter.reset();
        bodyFilter.reset();
    }

    if (vocalClarityActive)
    {
        const float clarityDb = (juce::jlimit(0.0f, 100.0f, clarityAmount.load()) - 50.0f) * 0.06f;
        const float airDb = (juce::jlimit(0.0f, 100.0f, clarityAir.load()) - 50.0f) * 0.05f;
        if (std::abs(clarityDb - lastClarityGainDb) > 0.001f)
        {
            const auto coefficients = makePeakingCoefficients(sampleRate, 3200.0f, clarityDb, 0.9f);
            clarityFilter.setCoefficients(coefficients[0], coefficients[1],
                coefficients[2], coefficients[3], coefficients[4]);
            lastClarityGainDb = clarityDb;
        }
        if (std::abs(airDb - lastClarityAirGainDb) > 0.001f)
        {
            const auto coefficients = makeHighShelfCoefficients(sampleRate, 8500.0f, airDb);
            clarityAirFilter.setCoefficients(coefficients[0], coefficients[1],
                coefficients[2], coefficients[3], coefficients[4]);
            lastClarityAirGainDb = airDb;
        }
    }
    else
    {
        clarityFilter.reset();
        clarityAirFilter.reset();
    }

    const float breathSensitivityValue = juce::jlimit(0.0f, 100.0f, breathSensitivity.load());
    const float breathReductionValue = juce::jlimit(0.0f, 100.0f, breathReduction.load());
    const float breathThresholdDb = -38.0f - breathSensitivityValue * 0.14f;
    const float breathMaximumReductionDb = breathReductionValue * 0.084f;
    const float breathDetectorAttack = std::exp(-1.0f / (sampleRate * 0.004f));
    const float breathDetectorRelease = std::exp(-1.0f / (sampleRate * 0.090f));
    const float breathVoiceAttack = std::exp(-1.0f / (sampleRate * 0.012f));
    const float breathVoiceRelease = std::exp(-1.0f / (sampleRate * 0.100f));
    const float breathGainAttack = std::exp(-1.0f / (sampleRate * 0.008f));
    const float breathGainRelease = std::exp(-1.0f / (sampleRate * 0.140f));
    if (!breathControlActive)
    {
        breathDetectorEnvelope = 0.0f;
        breathVoiceEnvelope = 0.0f;
        breathReductionGain = 1.0f;
        breathDetectorFilter.reset();
        breathBandFilter.reset();
    }

    const float gateThresholdDb = noiseGateThresholdDb.load();
    const float gateHoldMs = noiseGateHoldMs.load();
    const float gateReleaseMs = juce::jmax(10.0f, noiseGateReleaseMs.load());
    const float gateDetectorAttack = std::exp(-1.0f / (sampleRate * 0.002f));
    const float gateDetectorRelease = std::exp(-1.0f / (sampleRate * 0.030f));
    const float gateOpenCoefficient = std::exp(-1.0f / (sampleRate * 0.005f));
    const float gateCloseCoefficient = std::exp(-1.0f / (sampleRate * gateReleaseMs * 0.001f));
    const int gateHoldSamples = juce::roundToInt(gateHoldMs * sampleRate * 0.001f);

    if (!noiseGateActive)
    {
        noiseGateEnvelope = 0.0f;
        noiseGateGain = 1.0f;
        noiseGateHoldCounter = 0;
    }

    if (toneActive)
    {
        const float gains[] { toneBassDb.load(), toneMidDb.load(), toneTrebleDb.load() };
        for (int i = 0; i < 3; ++i)
        {
            if (std::abs(gains[i] - lastToneGainDb[(size_t)i]) > 0.001f)
            {
                BiquadCoefficients coefficients;
                if (i == 0)
                    coefficients = makeLowShelfCoefficients(sampleRate, 120.0f, gains[i]);
                else if (i == 1)
                    coefficients = makePeakingCoefficients(sampleRate, 1000.0f, gains[i], 0.7f);
                else
                    coefficients = makeHighShelfCoefficients(sampleRate, 8000.0f, gains[i]);

                toneFilters[(size_t)i].setCoefficients(coefficients[0], coefficients[1],
                    coefficients[2], coefficients[3], coefficients[4]);
                lastToneGainDb[(size_t)i] = gains[i];
            }
        }
    }
    else
    {
        for (auto& filter : toneFilters)
            filter.reset();
    }

    if (reverbActive)
    {
        const float mix = juce::jlimit(0.0f, 100.0f, reverbMixPercent.load());
        const float room = juce::jlimit(0.0f, 100.0f, reverbRoomPercent.load());
        const float time = juce::jlimit(0.1f, 5.0f, reverbTimeSeconds.load());
        if (!reverbWasActive || std::abs(mix - lastReverbMixPercent) > 0.05f
            || std::abs(room - lastReverbRoomPercent) > 0.05f
            || std::abs(time - lastReverbTimeSeconds) > 0.005f)
        {
            juce::Reverb::Parameters parameters;
            const float timeRatio = (time - 0.1f) / 4.9f;
            const float roomRatio = room / 100.0f;
            parameters.roomSize = 0.15f + timeRatio * 0.80f;
            parameters.damping = 0.85f - roomRatio * 0.60f;
            parameters.width = roomRatio;
            parameters.wetLevel = (mix / 100.0f) * 0.5f;
            parameters.dryLevel = 1.0f - parameters.wetLevel;
            parameters.freezeMode = 0.0f;
            reverb.setParameters(parameters);
            lastReverbMixPercent = mix;
            lastReverbRoomPercent = room;
            lastReverbTimeSeconds = time;
        }
        reverbWasActive = true;
    }
    else if (reverbWasActive)
    {
        reverb.reset();
        reverbWasActive = false;
    }

    auto* left = buffer.getWritePointer(0, info.startSample);
    for (int i = 0; i < info.numSamples; ++i)
    {
        float sample = input[i] * channelGain;
        sample = pitchShifter.process(sample, pitchTargetRatio, sampleRate,
                                      pitchSmoothingMs, pitchCorrectionActive);

        if (noiseGateActive)
        {
            const float detectorLevel = std::abs(sample);
            const float detectorCoefficient = detectorLevel > noiseGateEnvelope
                ? gateDetectorAttack : gateDetectorRelease;
            noiseGateEnvelope = detectorCoefficient * noiseGateEnvelope
                              + (1.0f - detectorCoefficient) * detectorLevel;

            const float envelopeDb = juce::Decibels::gainToDecibels(
                noiseGateEnvelope, -100.0f);
            if (envelopeDb >= gateThresholdDb)
                noiseGateHoldCounter = gateHoldSamples;
            else if (noiseGateHoldCounter > 0)
                --noiseGateHoldCounter;

            const float targetGain = (envelopeDb >= gateThresholdDb
                                      || noiseGateHoldCounter > 0)
                ? 1.0f : 0.001f;
            const float gainCoefficient = targetGain > noiseGateGain
                ? gateOpenCoefficient : gateCloseCoefficient;
            noiseGateGain = gainCoefficient * noiseGateGain
                          + (1.0f - gainCoefficient) * targetGain;
            sample *= noiseGateGain;
        }

        if (eqActive)
            for (auto& filter : eqFilters)
                sample = filter.process(sample);

        if (compressorActive)
        {
            const float detectorLevel = std::abs(sample);
            const float coefficient = detectorLevel > compressorEnvelope
                ? attackCoefficient : releaseCoefficient;
            compressorEnvelope = coefficient * compressorEnvelope
                               + (1.0f - coefficient) * detectorLevel;

            const float envelopeDb = juce::Decibels::gainToDecibels(
                compressorEnvelope, -100.0f);
            float gainReductionDb = 0.0f;
            if (envelopeDb > thresholdDb)
            {
                const float compressedDb = thresholdDb
                    + (envelopeDb - thresholdDb) / ratio;
                gainReductionDb = juce::jlimit(0.0f, 24.0f,
                                                envelopeDb - compressedDb);
            }

            blockGainReductionDb = juce::jmax(blockGainReductionDb, gainReductionDb);
            sample *= juce::Decibels::decibelsToGain(-gainReductionDb);
        }

        if (deEsserActive)
        {
            const float sibilanceBand = deEsserBandFilter.process(sample);
            const float detectorSample = deEsserDetectorFilter.process(sample);
            const float detectorLevel = std::abs(detectorSample);
            const float coefficient = detectorLevel > deEsserEnvelope
                ? deEsserAttackCoefficient : deEsserReleaseCoefficient;
            deEsserEnvelope = coefficient * deEsserEnvelope
                            + (1.0f - coefficient) * detectorLevel;

            const float sibilanceDb = juce::Decibels::gainToDecibels(
                deEsserEnvelope, -100.0f);
            if (sibilanceDb > deEsserThreshold)
            {
                const float excessDb = sibilanceDb - deEsserThreshold;
                const float reductionDb = juce::jlimit(0.0f, 12.0f, excessDb * 0.75f);
                const float attenuation = 1.0f
                    - juce::Decibels::decibelsToGain(-reductionDb);
                sample -= sibilanceBand * attenuation;
            }
        }

        if (breathControlActive)
        {
            const float detectorSample = breathDetectorFilter.process(sample);
            const float breathBand = breathBandFilter.process(sample);
            const float detectorLevel = std::abs(detectorSample);
            const float detectorCoefficient = detectorLevel > breathDetectorEnvelope
                ? breathDetectorAttack : breathDetectorRelease;
            breathDetectorEnvelope = detectorCoefficient * breathDetectorEnvelope
                                   + (1.0f - detectorCoefficient) * detectorLevel;

            const float voiceLevel = std::abs(sample);
            const float voiceCoefficient = voiceLevel > breathVoiceEnvelope
                ? breathVoiceAttack : breathVoiceRelease;
            breathVoiceEnvelope = voiceCoefficient * breathVoiceEnvelope
                                + (1.0f - voiceCoefficient) * voiceLevel;

            const float breathLevelDb = juce::Decibels::gainToDecibels(
                breathDetectorEnvelope, -100.0f);
            const float voiceLevelDb = juce::Decibels::gainToDecibels(
                breathVoiceEnvelope, -100.0f);
            const bool likelyBreath = breathLevelDb > breathThresholdDb
                                   && voiceLevelDb < -25.0f;
            const float excess = likelyBreath
                ? juce::jlimit(0.0f, 1.0f, (breathLevelDb - breathThresholdDb) / 18.0f)
                : 0.0f;
            const float targetReductionDb = breathMaximumReductionDb * excess;
            const float targetGain = juce::Decibels::decibelsToGain(-targetReductionDb);
            const float gainCoefficient = targetGain < breathReductionGain
                ? breathGainAttack : breathGainRelease;
            breathReductionGain = gainCoefficient * breathReductionGain
                                + (1.0f - gainCoefficient) * targetGain;
            sample -= breathBand * (1.0f - breathReductionGain);
        }

        if (voiceEnhanceActive)
            for (auto& filter : voiceEnhanceFilters)
                sample = filter.process(sample);

        if (warmthBodyActive)
        {
            sample = warmthFilter.process(sample);
            sample = bodyFilter.process(sample);
        }

        if (vocalClarityActive)
        {
            sample = clarityFilter.process(sample);
            sample = clarityAirFilter.process(sample);
        }

        const float exciterBand = vocalExciterFilter.process(sample);
        vocalExciterMixGain = exciterMixSmoothing * vocalExciterMixGain
                            + (1.0f - exciterMixSmoothing) * exciterTargetMix;
        if (vocalExciterMixGain > 0.00001f)
        {
            const float saturatedBand = std::tanh(exciterBand * exciterDrive)
                                      / juce::jmax(0.001f, exciterNormaliser);
            sample += (saturatedBand - exciterBand) * vocalExciterMixGain;
        }

        if (toneActive)
            for (auto& filter : toneFilters)
                sample = filter.process(sample);

        if (vocalSmoothActive)
        {
            const float detectorSample = vocalSmoothDetectorFilter.process(sample);
            const float resonanceBand = vocalSmoothBandFilter.process(sample);
            const float detectorLevel = std::abs(detectorSample);
            const float detectorCoefficient = detectorLevel > vocalSmoothEnvelope
                ? smoothDetectorAttack : smoothDetectorRelease;
            vocalSmoothEnvelope = detectorCoefficient * vocalSmoothEnvelope
                                + (1.0f - detectorCoefficient) * detectorLevel;

            const float resonanceDb = juce::Decibels::gainToDecibels(
                vocalSmoothEnvelope, -100.0f);
            const float excess = juce::jlimit(0.0f, 1.0f,
                (resonanceDb - smoothThresholdDb) / 12.0f);
            const float reductionDb = smoothMaximumReductionDb * excess;
            const float targetGain = juce::Decibels::decibelsToGain(-reductionDb);
            const float gainCoefficient = targetGain < vocalSmoothGain
                ? smoothGainAttack : smoothGainRelease;
            vocalSmoothGain = gainCoefficient * vocalSmoothGain
                            + (1.0f - gainCoefficient) * targetGain;
            sample -= resonanceBand * (1.0f - vocalSmoothGain);
        }
        else
        {
            vocalSmoothEnvelope = 0.0f;
            vocalSmoothGain = 1.0f;
            vocalSmoothDetectorFilter.reset();
            vocalSmoothBandFilter.reset();
        }

        if (vocalLevelingActive)
        {
            const float level = std::abs(sample);
            const float envelopeCoefficient = level > vocalLevelEnvelope
                ? vocalEnvelopeAttack : vocalEnvelopeRelease;
            vocalLevelEnvelope = envelopeCoefficient * vocalLevelEnvelope
                               + (1.0f - envelopeCoefficient) * level;

            const float envelopeDb = juce::Decibels::gainToDecibels(
                vocalLevelEnvelope, -100.0f);
            float targetGain = 1.0f;
            if (envelopeDb > -48.0f)
                targetGain = vocalTargetLinear / juce::jmax(1.0e-5f, vocalLevelEnvelope);

            targetGain = juce::jlimit(vocalMinimumGain, vocalMaximumBoost, targetGain);
            const float gainCoefficient = targetGain > vocalLevelGain
                ? vocalGainUp : vocalGainDown;
            vocalLevelGain = gainCoefficient * vocalLevelGain
                           + (1.0f - gainCoefficient) * targetGain;
        }
        else
        {
            vocalLevelGain = vocalGainReturn * vocalLevelGain
                           + (1.0f - vocalGainReturn);
        }

        sample *= vocalLevelGain;
        left[i] = sample * masterGain;
    }

    if (reverbActive)
        reverb.processMono(left, info.numSamples);

    // Final sample-peak limiter after all processing, Reverb and Master Volume.
    const bool limiterActive = useGainStage && vocalLimiterEnabled.load();
    const float limiterCeilingDb = juce::jlimit(-6.0f, -0.1f,
                                                vocalLimiterCeilingDb.load());
    const float limiterCeiling = juce::Decibels::decibelsToGain(limiterCeilingDb);
    const float limiterReleaseSeconds = juce::jlimit(20.0f, 300.0f,
        vocalLimiterReleaseMs.load()) * 0.001f;
    const float limiterReleaseCoefficient = std::exp(
        -1.0f / ((float)currentSampleRate * limiterReleaseSeconds));

    for (int i = 0; i < info.numSamples; ++i)
    {
        const float sample = left[i];
        const float magnitude = std::abs(sample);
        const float targetGain = limiterActive && magnitude > limiterCeiling
            ? limiterCeiling / juce::jmax(magnitude, 1.0e-9f)
            : 1.0f;

        if (targetGain < vocalLimiterGain)
            vocalLimiterGain = targetGain;
        else
            vocalLimiterGain = limiterReleaseCoefficient * vocalLimiterGain
                             + (1.0f - limiterReleaseCoefficient);

        left[i] = sample * vocalLimiterGain;
    }

    compressorGainReductionDb.store(compressorActive ? blockGainReductionDb : 0.0f);

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
            info.setText(
                "Choose a Windows recording input such as Stereo Mix or What U Hear below, if your driver provides it. "
                "The main window selects the playback output device.",
                juce::dontSendNotification);
            info.setFont(juce::FontOptions(13.0f));
            info.setColour(juce::Label::textColourId, juce::Colour(190, 205, 220));
            info.setJustificationType(juce::Justification::centredLeft);
            addAndMakeVisible(info);
            addAndMakeVisible(selector);
            setSize(760, 600);
        }

        void resized() override
        {
            auto area = getLocalBounds().reduced(14);
            info.setBounds(area.removeFromTop(58));
            area.removeFromTop(6);
            selector.setBounds(area);
        }

    private:
        juce::Label info;
        juce::AudioDeviceSelectorComponent selector;
    };
}

void MainComponent::refreshAudioOutputDevices()
{
    auto* deviceType = deviceManager.getCurrentDeviceTypeObject();
    if (deviceType == nullptr)
        return;

    const auto outputNames = deviceType->getDeviceNames(false);
    if (outputNames.joinIntoString("\n") != lastAudioOutputNames.joinIntoString("\n"))
    {
        lastAudioOutputNames = outputNames;
        const auto setup = deviceManager.getAudioDeviceSetup();
        const auto selectedName = setup.outputDeviceName;

        deviceSelector.clear(juce::dontSendNotification);
        for (int i = 0; i < outputNames.size(); ++i)
            deviceSelector.addItem(outputNames[i], i + 1);

        if (outputNames.isEmpty())
        {
            deviceSelector.addItem("No playback devices found", 1);
            deviceSelector.setEnabled(false);
        }
        else
        {
            deviceSelector.setEnabled(true);
            int selectedId = 0;
            for (int i = 0; i < outputNames.size(); ++i)
                if (outputNames[i] == selectedName)
                    selectedId = i + 1;

            if (selectedId == 0)
                selectedId = 1;
            deviceSelector.setSelectedId(selectedId, juce::dontSendNotification);
        }
    }

    const auto setup = deviceManager.getAudioDeviceSetup();
    if (!setup.outputDeviceName.isEmpty())
    {
        for (int i = 0; i < lastAudioOutputNames.size(); ++i)
            if (lastAudioOutputNames[i] == setup.outputDeviceName
                && deviceSelector.getSelectedId() != i + 1)
                deviceSelector.setSelectedId(i + 1, juce::dontSendNotification);
    }
}

void MainComponent::selectAudioOutputDevice()
{
    const int selectedIndex = deviceSelector.getSelectedId() - 1;
    if (selectedIndex < 0 || selectedIndex >= lastAudioOutputNames.size())
        return;

    auto setup = deviceManager.getAudioDeviceSetup();
    setup.outputDeviceName = lastAudioOutputNames[selectedIndex];
    const auto error = deviceManager.setAudioDeviceSetup(setup, true);

    if (error.isEmpty())
    {
        statusLabel.setText("Playback output: " + lastAudioOutputNames[selectedIndex],
                            juce::dontSendNotification);
        connectionLabel.setText(juce::String::fromUTF8("ĐẦU RA SẴN SÀNG  |  ĐÃ CHỌN THIẾT BỊ"),
                                juce::dontSendNotification);
        connectionLabel.setColour(juce::Label::textColourId, green);
    }
    else
    {
        statusLabel.setText(juce::String::fromUTF8("Không thể mở thiết bị đầu ra: ") + error,
                            juce::dontSendNotification);
        refreshAudioOutputDevices();
    }
}

void MainComponent::showAudioSettings()
{
    juce::DialogWindow::LaunchOptions options;
    options.content.setOwned(new AudioSettingsContent(deviceManager));
    options.dialogTitle = juce::String::fromUTF8("Cài đặt thiết bị âm thanh");
    options.dialogBackgroundColour = juce::Colour(18, 25, 35);
    options.escapeKeyTriggersCloseButton = true;
    options.useNativeTitleBar = true;
    options.resizable = true;
    options.launchAsync();
}

void MainComponent::showPitchCorrectionModule()
{
    juce::DialogWindow::LaunchOptions options;
    options.content.setOwned(new PitchCorrectionModuleContent(
        pitchSongKey, pitchScale, pitchCorrectionAmount, pitchCorrectionSpeed,
        pitchDetectedMidi, pitchTargetMidi, pitchDeviationCents,
        pitchCorrectionEnabled));
    options.dialogTitle = "Pitch Correction Module";
    options.dialogBackgroundColour = juce::Colour(13, 19, 28);
    options.escapeKeyTriggersCloseButton = true;
    options.useNativeTitleBar = true;
    options.resizable = true;
    options.launchAsync();
}

void MainComponent::showVocalLevelingModule()
{
    juce::DialogWindow::LaunchOptions options;
    options.content.setOwned(new VocalLevelingModuleContent(
        vocalLevelingEnabled, vocalLevelTargetDb,
        vocalLevelMaxBoostDb, vocalLevelMaxCutDb));
    options.dialogTitle = "Vocal Leveling Module";
    options.dialogBackgroundColour = juce::Colour(13, 19, 28);
    options.escapeKeyTriggersCloseButton = true;
    options.useNativeTitleBar = true;
    options.resizable = true;
    options.launchAsync();
}

void MainComponent::showWarmthBodyModule()
{
    juce::DialogWindow::LaunchOptions options;
    options.content.setOwned(new WarmthBodyModuleContent(
        warmthBodyEnabled, warmthAmount, bodyAmount));
    options.dialogTitle = "Warmth & Body Module";
    options.dialogBackgroundColour = juce::Colour(13, 19, 28);
    options.escapeKeyTriggersCloseButton = true;
    options.useNativeTitleBar = true;
    options.resizable = true;
    options.launchAsync();
}

void MainComponent::showVocalClarityModule()
{
    juce::DialogWindow::LaunchOptions options;
    options.content.setOwned(new VocalClarityModuleContent(
        vocalClarityEnabled, clarityAmount, clarityAir));
    options.dialogTitle = "Vocal Clarity Module";
    options.dialogBackgroundColour = juce::Colour(13, 19, 28);
    options.escapeKeyTriggersCloseButton = true;
    options.useNativeTitleBar = true;
    options.resizable = true;
    options.launchAsync();
}

void MainComponent::showBreathControlModule()
{
    juce::DialogWindow::LaunchOptions options;
    options.content.setOwned(new BreathControlModuleContent(
        breathControlEnabled, breathSensitivity, breathReduction));
    options.dialogTitle = "Breath Control Module";
    options.dialogBackgroundColour = juce::Colour(13, 19, 28);
    options.escapeKeyTriggersCloseButton = true;
    options.useNativeTitleBar = true;
    options.resizable = true;
    options.launchAsync();
}


void MainComponent::showVocalExciterModule()
{
    juce::DialogWindow::LaunchOptions options;
    options.content.setOwned(new VocalExciterModuleContent(
        vocalExciterEnabled, vocalExciterAmount, vocalExciterMix));
    options.dialogTitle = "Vocal Exciter Module";
    options.dialogBackgroundColour = juce::Colour(13, 19, 28);
    options.escapeKeyTriggersCloseButton = true;
    options.useNativeTitleBar = true;
    options.resizable = true;
    options.launchAsync();
}



void MainComponent::showVocalSmoothModule()
{
    juce::DialogWindow::LaunchOptions options;
    options.content.setOwned(new VocalSmoothModuleContent(
        vocalSmoothEnabled, vocalSmoothSensitivity, vocalSmoothIntensity));
    options.dialogTitle = "Vocal Smooth Module";
    options.dialogBackgroundColour = juce::Colour(13, 19, 28);
    options.escapeKeyTriggersCloseButton = true;
    options.useNativeTitleBar = true;
    options.resizable = true;
    options.launchAsync();
}

void MainComponent::showVocalLimiterModule()
{
    juce::DialogWindow::LaunchOptions options;
    options.content.setOwned(new VocalLimiterModuleContent(
        vocalLimiterEnabled, vocalLimiterCeilingDb, vocalLimiterReleaseMs));
    options.dialogTitle = "Vocal Limiter Module";
    options.dialogBackgroundColour = juce::Colour(13, 19, 28);
    options.escapeKeyTriggersCloseButton = true;
    options.useNativeTitleBar = true;
    options.resizable = true;
    options.launchAsync();
}

void MainComponent::showPlannedVocalModule(const juce::String& moduleName)
{
    juce::DialogWindow::LaunchOptions options;
    options.content.setOwned(new PlannedVocalModuleContent(moduleName));
    options.dialogTitle = moduleName;
    options.dialogBackgroundColour = juce::Colour(13, 19, 28);
    options.escapeKeyTriggersCloseButton = true;
    options.useNativeTitleBar = true;
    options.resizable = true;
    options.launchAsync();
}

void MainComponent::timerCallback()
{
    refreshAudioOutputDevices();

    auto* device = deviceManager.getCurrentAudioDevice();
    const juce::String deviceName = device != nullptr ? device->getName()
                                                       : juce::String::fromUTF8("Chưa mở thiết bị âm thanh");

    if (deviceName != lastAudioDeviceName)
    {
        lastAudioDeviceName = deviceName;
        const bool ready = device != nullptr;
        connectionLabel.setText(ready ? juce::String::fromUTF8("ÂM THANH SẴN SÀNG  |  ĐẦU RA ĐANG HOẠT ĐỘNG")
                                      : "NO DEVICE  |  CHECK AUDIO SETTINGS",
                                juce::dontSendNotification);
        connectionLabel.setColour(juce::Label::textColourId, ready ? green
                                                                    : juce::Colour(255, 110, 90));
    }

    repaint();
}
