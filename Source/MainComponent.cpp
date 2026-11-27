#include "MainComponent.h"

//==============================================================================
namespace
{
    const juce::Colour backgroundColour { 17, 21, 29 };
    const juce::Colour panelColour { 29, 36, 49 };
    const juce::Colour panelEdgeColour { 52, 64, 82 };
    const juce::Colour accentColour { 54, 174, 235 };
    const juce::Colour textColour { 235, 240, 247 };
    const juce::Colour mutedColour { 151, 164, 181 };
}

KnobControl::KnobControl(const juce::String& title,
                         double minimum,
                         double maximum,
                         double initial,
                         const juce::String& suffix)
    : unitSuffix(suffix)
{
    titleLabel.setText(title, juce::dontSendNotification);
    titleLabel.setJustificationType(juce::Justification::centred);
    titleLabel.setFont(juce::FontOptions(12.5f, juce::Font::bold));
    titleLabel.setColour(juce::Label::textColourId, textColour);
    addAndMakeVisible(titleLabel);

    slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    slider.setRange(minimum, maximum, 0.1);
    slider.setValue(initial, juce::dontSendNotification);
    slider.setColour(juce::Slider::rotarySliderFillColourId, accentColour);
    slider.setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colour(67, 79, 98));
    slider.setColour(juce::Slider::thumbColourId, juce::Colours::white);
    slider.setMouseDragSensitivity(180);
    addAndMakeVisible(slider);

    valueLabel.setJustificationType(juce::Justification::centred);
    valueLabel.setFont(juce::FontOptions(12.0f, juce::Font::bold));
    valueLabel.setColour(juce::Label::textColourId, accentColour);
    addAndMakeVisible(valueLabel);

    slider.onValueChange = [this]
    {
        valueLabel.setText(juce::String(slider.getValue(), 1) + unitSuffix,
                           juce::dontSendNotification);
    };
    valueLabel.setText(juce::String(initial, 1) + unitSuffix,
                       juce::dontSendNotification);
}

void KnobControl::paint(juce::Graphics& g)
{
    juce::ignoreUnused(g);
}

void KnobControl::resized()
{
    auto area = getLocalBounds();
    titleLabel.setBounds(area.removeFromTop(24));
    valueLabel.setBounds(area.removeFromBottom(22));
    slider.setBounds(area.reduced(2));
}

//==============================================================================
MainComponent::MainComponent()
{
    setSize(1180, 820);

    titleLabel.setText("VOICE VOCAL STUDIO", juce::dontSendNotification);
    titleLabel.setFont(juce::FontOptions(27.0f, juce::Font::bold));
    titleLabel.setColour(juce::Label::textColourId, accentColour);
    addAndMakeVisible(titleLabel);

    subtitleLabel.setText("BÀN ĐIỀU KHIỂN GIỌNG HÁT THỜI GIAN THỰC",
                         juce::dontSendNotification);
    subtitleLabel.setFont(juce::FontOptions(12.0f));
    subtitleLabel.setColour(juce::Label::textColourId, mutedColour);
    addAndMakeVisible(subtitleLabel);

    deviceLabel.setText("THIẾT BỊ ÂM THANH", juce::dontSendNotification);
    deviceLabel.setFont(juce::FontOptions(13.0f, juce::Font::bold));
    deviceLabel.setColour(juce::Label::textColourId, textColour);
    addAndMakeVisible(deviceLabel);

    inputLabel.setText("ĐẦU VÀO", juce::dontSendNotification);
    outputLabel.setText("ĐẦU RA", juce::dontSendNotification);
    for (auto* label : { &inputLabel, &outputLabel })
    {
        label->setFont(juce::FontOptions(10.5f, juce::Font::bold));
        label->setColour(juce::Label::textColourId, mutedColour);
        addAndMakeVisible(*label);
    }

    inputDeviceBox.addItem("Chưa kết nối thiết bị", 1);
    inputDeviceBox.setSelectedId(1);
    outputDeviceBox.addItem("Chưa kết nối thiết bị", 1);
    outputDeviceBox.setSelectedId(1);
    for (auto* box : { &inputDeviceBox, &outputDeviceBox })
    {
        box->setColour(juce::ComboBox::backgroundColourId, juce::Colour(39, 48, 63));
        box->setColour(juce::ComboBox::textColourId, textColour);
        box->setColour(juce::ComboBox::outlineColourId, panelEdgeColour);
        addAndMakeVisible(*box);
    }

    engineLabel.setText("AUDIO ENGINE: CHƯA KẾT NỐI", juce::dontSendNotification);
    engineLabel.setFont(juce::FontOptions(10.5f, juce::Font::bold));
    engineLabel.setColour(juce::Label::textColourId, juce::Colour(242, 183, 91));
    addAndMakeVisible(engineLabel);

    const std::array<juce::String, 4> names {
        "MIXER & MỨC ÂM", "EQUALIZER 3 DẢI", "COMPRESSOR", "LÀM SẠCH & MÀU GIỌNG"
    };
    const std::array<juce::String, 4> switches {
        "Bật Mixer", "Bật EQ", "Bật Compressor", "Bật xử lý"
    };

    for (int i = 0; i < 4; ++i)
    {
        sectionTitles[(size_t)i].setText(names[(size_t)i], juce::dontSendNotification);
        sectionTitles[(size_t)i].setFont(juce::FontOptions(14.0f, juce::Font::bold));
        sectionTitles[(size_t)i].setColour(juce::Label::textColourId, textColour);
        addAndMakeVisible(sectionTitles[(size_t)i]);

        sectionToggles[(size_t)i].setButtonText(switches[(size_t)i]);
        sectionToggles[(size_t)i].setColour(juce::ToggleButton::textColourId, mutedColour);
        sectionToggles[(size_t)i].setToggleState(i == 0, juce::dontSendNotification);
        addAndMakeVisible(sectionToggles[(size_t)i]);
    }

    addKnob("Input Gain", -12.0, 24.0, 0.0, " dB", 0);
    addKnob("Mic Volume", 0.0, 100.0, 75.0, " %", 0);
    addKnob("Output", 0.0, 100.0, 80.0, " %", 0);

    addKnob("Bass", -12.0, 12.0, 2.0, " dB", 1);
    addKnob("Mid", -12.0, 12.0, 0.0, " dB", 1);
    addKnob("Treble", -12.0, 12.0, 1.0, " dB", 1);

    addKnob("Threshold", -60.0, 0.0, -18.0, " dB", 2);
    addKnob("Ratio", 1.0, 10.0, 3.0, ":1", 2);
    addKnob("Attack", 1.0, 100.0, 10.0, " ms", 2);
    addKnob("Release", 10.0, 500.0, 120.0, " ms", 2);

    addKnob("De-esser", 0.0, 100.0, 25.0, " %", 3);
    addKnob("Saturation", 0.0, 100.0, 12.0, " %", 3);
    addKnob("Noise Reduce", 0.0, 100.0, 15.0, " %", 3);
    addKnob("Limiter", -12.0, 0.0, -1.0, " dB", 3);

    presetBox.addItem("Karaoke - Giọng hát", 1);
    presetBox.addItem("MC / Trò chuyện", 2);
    presetBox.addItem("Podcast", 3);
    presetBox.addItem("Livestream", 4);
    presetBox.setSelectedId(1);
    presetBox.setColour(juce::ComboBox::backgroundColourId, juce::Colour(39, 48, 63));
    presetBox.setColour(juce::ComboBox::textColourId, textColour);
    presetBox.setColour(juce::ComboBox::outlineColourId, panelEdgeColour);
    addAndMakeVisible(presetBox);

    inputMeterLabel.setText("INPUT", juce::dontSendNotification);
    outputMeterLabel.setText("OUTPUT", juce::dontSendNotification);
    inputMeterState.setText("Chưa có tín hiệu", juce::dontSendNotification);
    outputMeterState.setText("Chưa có tín hiệu", juce::dontSendNotification);
    for (auto* label : { &inputMeterLabel, &outputMeterLabel })
    {
        label->setFont(juce::FontOptions(10.5f, juce::Font::bold));
        label->setColour(juce::Label::textColourId, mutedColour);
        addAndMakeVisible(*label);
    }
    for (auto* label : { &inputMeterState, &outputMeterState })
    {
        label->setFont(juce::FontOptions(11.0f));
        label->setColour(juce::Label::textColourId, juce::Colour(242, 183, 91));
        addAndMakeVisible(*label);
    }

    processButton.setClickingTogglesState(true);
    processButton.setColour(juce::TextButton::buttonColourId, juce::Colour(26, 132, 99));
    processButton.setColour(juce::TextButton::textColourOffId, juce::Colours::white);
    processButton.setColour(juce::TextButton::textColourOnId, juce::Colours::white);
    processButton.onClick = [this]
    {
        processingRequested = processButton.getToggleState();
        updateStatus(processingRequested
            ? "Đã chọn bật xử lý - chưa có audio engine/DSP"
            : "Đã tắt yêu cầu xử lý - chưa có audio engine/DSP");
    };
    addAndMakeVisible(processButton);

    bypassButton.setClickingTogglesState(true);
    bypassButton.setColour(juce::TextButton::buttonColourId, juce::Colour(91, 99, 113));
    bypassButton.setColour(juce::TextButton::textColourOffId, juce::Colours::white);
    bypassButton.setColour(juce::TextButton::textColourOnId, juce::Colours::white);
    bypassButton.onClick = [this]
    {
        bypassRequested = bypassButton.getToggleState();
        updateStatus(bypassRequested
            ? "Bypass được chọn trên giao diện - chưa tác động âm thanh"
            : "Đã bỏ chọn Bypass");
    };
    addAndMakeVisible(bypassButton);

    statusLabel.setText("BẢN GIAO DIỆN: CÁC ĐIỀU KHIỂN CHƯA KẾT NỐI DSP",
                       juce::dontSendNotification);
    statusLabel.setFont(juce::FontOptions(11.5f, juce::Font::bold));
    statusLabel.setColour(juce::Label::textColourId, juce::Colour(242, 183, 91));
    addAndMakeVisible(statusLabel);

    footerLabel.setText("VOICE VOCAL STUDIO  |  UI PROTOTYPE  |  CHƯA XỬ LÝ ÂM THANH",
                       juce::dontSendNotification);
    footerLabel.setFont(juce::FontOptions(10.5f));
    footerLabel.setColour(juce::Label::textColourId, mutedColour);
    addAndMakeVisible(footerLabel);
}

KnobControl* MainComponent::addKnob(const juce::String& title,
                                    double minimum,
                                    double maximum,
                                    double initial,
                                    const juce::String& suffix,
                                    int group)
{
    auto control = std::make_unique<KnobControl>(title, minimum, maximum, initial, suffix);
    auto* pointer = control.get();
    knobs.push_back(std::move(control));
    groupKnobs[(size_t)group].push_back(pointer);
    addAndMakeVisible(*pointer);
    return pointer;
}

void MainComponent::updateStatus(const juce::String& message)
{
    statusLabel.setText(message, juce::dontSendNotification);
}

//==============================================================================
void MainComponent::paint(juce::Graphics& g)
{
    g.fillAll(backgroundColour);

    auto drawPanel = [&g](juce::Rectangle<int> area)
    {
        g.setColour(panelColour);
        g.fillRoundedRectangle(area.toFloat(), 12.0f);
        g.setColour(panelEdgeColour);
        g.drawRoundedRectangle(area.toFloat(), 12.0f, 1.0f);
    };

    for (const auto& area : panelBounds)
        drawPanel(area);

    auto headerAccent = juce::Rectangle<float>(25.0f, 24.0f, 5.0f, 54.0f);
    g.setColour(accentColour);
    g.fillRoundedRectangle(headerAccent, 2.0f);


}

//==============================================================================
void MainComponent::resized()
{
    auto area = getLocalBounds().reduced(24);

    auto header = area.removeFromTop(78);
    titleLabel.setBounds(header.getX() + 16, header.getY(), header.getWidth() - 16, 42);
    subtitleLabel.setBounds(getLocalBounds().getX() + 40, 62, 600, 22);

    area.removeFromTop(10);
    auto deviceArea = area.removeFromTop(112);
    deviceLabel.setBounds(deviceArea.getX() + 16, deviceArea.getY() + 8, 220, 22);

    const int half = (deviceArea.getWidth() - 48) / 2;
    inputLabel.setBounds(deviceArea.getX() + 16, deviceArea.getY() + 34, 100, 18);
    inputDeviceBox.setBounds(deviceArea.getX() + 16, deviceArea.getY() + 55, half - 12, 30);
    outputLabel.setBounds(deviceArea.getX() + half + 32, deviceArea.getY() + 34, 100, 18);
    outputDeviceBox.setBounds(deviceArea.getX() + half + 32, deviceArea.getY() + 55, half - 12, 30);
    engineLabel.setBounds(deviceArea.getRight() - 300, deviceArea.getY() + 8, 280, 22);

    area.removeFromTop(12);
    auto meterArea = area.removeFromTop(48);
    inputMeterLabel.setBounds(meterArea.getX() + 16, meterArea.getY(), 70, 18);
    inputMeterState.setBounds(meterArea.getX() + 90, meterArea.getY(), 240, 18);
    outputMeterLabel.setBounds(meterArea.getX() + 410, meterArea.getY(), 70, 18);
    outputMeterState.setBounds(meterArea.getX() + 485, meterArea.getY(), 240, 18);

    area.removeFromTop(12);
    const int gap = 14;
    const int panelWidth = (area.getWidth() - gap) / 2;
    const int panelHeight = (area.getHeight() - gap - 105) / 2;

    panelBounds[0] = { area.getX(), area.getY(), panelWidth, panelHeight };
    panelBounds[1] = { area.getX() + panelWidth + gap, area.getY(), panelWidth, panelHeight };
    panelBounds[2] = { area.getX(), area.getY() + panelHeight + gap, panelWidth, panelHeight };
    panelBounds[3] = { area.getX() + panelWidth + gap, area.getY() + panelHeight + gap, panelWidth, panelHeight };

    for (int i = 0; i < 4; ++i)
    {
        auto panel = panelBounds[(size_t)i];
        sectionTitles[(size_t)i].setBounds(panel.getX() + 16, panel.getY() + 12, panel.getWidth() - 170, 24);
        sectionToggles[(size_t)i].setBounds(panel.getRight() - 145, panel.getY() + 10, 132, 26);

        const auto& controls = groupKnobs[(size_t)i];
        const int count = (int)controls.size();
        const int innerWidth = panel.getWidth() - 28;
        const int cellWidth = innerWidth / juce::jmax(1, count);
        const int top = panel.getY() + 48;
        const int availableHeight = panel.getHeight() - 58;
        for (int k = 0; k < count; ++k)
        {
            const int x = panel.getX() + 14 + k * cellWidth;
            controls[(size_t)k]->setBounds(x, top, cellWidth, availableHeight);
        }
    }

    auto bottom = juce::Rectangle<int>(area.getX(), area.getBottom() - 91, area.getWidth(), 82);
    presetBox.setBounds(bottom.getX() + 8, bottom.getY() + 4, 210, 30);
    processButton.setBounds(bottom.getX() + 236, bottom.getY() + 4, 150, 34);
    bypassButton.setBounds(bottom.getX() + 398, bottom.getY() + 4, 160, 34);
    statusLabel.setBounds(bottom.getX() + 8, bottom.getY() + 43, bottom.getWidth() - 16, 22);
    footerLabel.setBounds(24, getHeight() - 23, getWidth() - 48, 16);
}
