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

//==============================================================================
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

    // Use a standard horizontal fader: reliable JUCE rendering, no custom rotary LookAndFeel.
    slider.setSliderStyle(juce::Slider::LinearHorizontal);
    slider.setColour(juce::Slider::trackColourId, accentColour);
    slider.setColour(juce::Slider::backgroundColourId, juce::Colour(15, 20, 29));
    slider.setColour(juce::Slider::thumbColourId, juce::Colours::white);
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

KnobControl::~KnobControl() = default;

void KnobControl::paint(juce::Graphics& g)
{
    auto area = getLocalBounds().toFloat().reduced(1.0f);
    g.setColour(panelEdgeColour);
    g.drawRoundedRectangle(area, 5.0f, 1.0f);
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
    // Use a Windows UI font with full Vietnamese glyph coverage.
    juce::LookAndFeel::getDefaultLookAndFeel().setDefaultSansSerifTypefaceName("Segoe UI");

    setSize(820, 620);

    titleLabel.setText("VOICE VOCAL STUDIO - KIEM TRA BAN MOI 02", juce::dontSendNotification);
    titleLabel.setFont(juce::FontOptions(27.0f, juce::Font::bold));
    titleLabel.setColour(juce::Label::textColourId, accentColour);
    addAndMakeVisible(titleLabel);

    subtitleLabel.setText(juce::String::fromUTF8("\102\303\200\116\040\304\220\111\341\273\200\125\040\113\110\111\341\273\202\116\040\107\111\341\273\214\116\107\040\110\303\201\124\040\124\110\341\273\234\111\040\107\111\101\116\040\124\110\341\273\260\103"),
                         juce::dontSendNotification);
    subtitleLabel.setFont(juce::FontOptions(12.0f));
    subtitleLabel.setColour(juce::Label::textColourId, mutedColour);
    addAndMakeVisible(subtitleLabel);

    deviceLabel.setText(juce::String::fromUTF8("\124\110\111\341\272\276\124\040\102\341\273\212\040\303\202\115\040\124\110\101\116\110"), juce::dontSendNotification);
    deviceLabel.setFont(juce::FontOptions(13.0f, juce::Font::bold));
    deviceLabel.setColour(juce::Label::textColourId, textColour);
    addAndMakeVisible(deviceLabel);

    inputLabel.setText(juce::String::fromUTF8("\304\220\341\272\246\125\040\126\303\200\117"), juce::dontSendNotification);
    outputLabel.setText(juce::String::fromUTF8("\304\220\341\272\246\125\040\122\101"), juce::dontSendNotification);
    for (auto* label : { &inputLabel, &outputLabel })
    {
        label->setFont(juce::FontOptions(10.5f, juce::Font::bold));
        label->setColour(juce::Label::textColourId, mutedColour);
        addAndMakeVisible(*label);
    }

    inputDeviceBox.addItem(juce::String::fromUTF8("\103\150\306\260\141\040\153\341\272\277\164\040\156\341\273\221\151\040\164\150\151\341\272\277\164\040\142\341\273\213"), 1);
    inputDeviceBox.setSelectedId(1);
    outputDeviceBox.addItem(juce::String::fromUTF8("\103\150\306\260\141\040\153\341\272\277\164\040\156\341\273\221\151\040\164\150\151\341\272\277\164\040\142\341\273\213"), 1);
    outputDeviceBox.setSelectedId(1);
    for (auto* box : { &inputDeviceBox, &outputDeviceBox })
    {
        box->setColour(juce::ComboBox::backgroundColourId, juce::Colour(39, 48, 63));
        box->setColour(juce::ComboBox::textColourId, textColour);
        box->setColour(juce::ComboBox::outlineColourId, panelEdgeColour);
        addAndMakeVisible(*box);
    }

    engineLabel.setText(juce::String::fromUTF8("\101\125\104\111\117\040\105\116\107\111\116\105\072\040\103\110\306\257\101\040\113\341\272\276\124\040\116\341\273\220\111"), juce::dontSendNotification);
    engineLabel.setFont(juce::FontOptions(10.5f, juce::Font::bold));
    engineLabel.setColour(juce::Label::textColourId, juce::Colour(242, 183, 91));
    addAndMakeVisible(engineLabel);

    const std::array<juce::String, 4> names {
        juce::String::fromUTF8("\115\111\130\105\122\040\046\040\115\341\273\250\103\040\303\202\115"), juce::String::fromUTF8("\105\121\125\101\114\111\132\105\122\040\063\040\104\341\272\242\111"), "COMPRESSOR", juce::String::fromUTF8("\114\303\200\115\040\123\341\272\240\103\110\040\046\040\115\303\200\125\040\107\111\341\273\214\116\107")
    };
    const std::array<juce::String, 4> switches {
        juce::String::fromUTF8("\102\341\272\255\164\040\115\151\170\145\162"), juce::String::fromUTF8("\102\341\272\255\164\040\105\121"), juce::String::fromUTF8("\102\341\272\255\164\040\103\157\155\160\162\145\163\163\157\162"), juce::String::fromUTF8("\102\341\272\255\164\040\170\341\273\255\040\154\303\275")
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

    auto configureVolumeSlider = [this](juce::Slider& slider, juce::Label& label,
                                        const juce::String& caption, double initial)
    {
        label.setText(caption, juce::dontSendNotification);
        label.setFont(juce::FontOptions(11.5f, juce::Font::bold));
        label.setColour(juce::Label::textColourId, textColour);
        addAndMakeVisible(label);

        slider.setSliderStyle(juce::Slider::LinearHorizontal);
        slider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 42, 22);
        slider.setRange(0.0, 100.0, 1.0);
        slider.setValue(initial, juce::dontSendNotification);
        slider.setColour(juce::Slider::trackColourId, accentColour);
        slider.setColour(juce::Slider::thumbColourId, juce::Colours::white);
        slider.setColour(juce::Slider::textBoxTextColourId, textColour);
        slider.setColour(juce::Slider::textBoxBackgroundColourId, juce::Colour(39, 48, 63));
        slider.setColour(juce::Slider::textBoxOutlineColourId, panelEdgeColour);
        addAndMakeVisible(slider);
    };
    configureVolumeSlider(micVolumeSlider, micVolumeLabel, "MIC VOLUME", 75.0);
    configureVolumeSlider(outputVolumeSlider, outputVolumeLabel, "OUTPUT VOLUME", 80.0);

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

    presetBox.addItem(juce::String::fromUTF8("\113\141\162\141\157\153\145\040\055\040\107\151\341\273\215\156\147\040\150\303\241\164"), 1);
    presetBox.addItem(juce::String::fromUTF8("\115\103\040\057\040\124\162\303\262\040\143\150\165\171\341\273\207\156"), 2);
    presetBox.addItem("Podcast", 3);
    presetBox.addItem("Livestream", 4);
    presetBox.setSelectedId(1);
    presetBox.setColour(juce::ComboBox::backgroundColourId, juce::Colour(39, 48, 63));
    presetBox.setColour(juce::ComboBox::textColourId, textColour);
    presetBox.setColour(juce::ComboBox::outlineColourId, panelEdgeColour);
    addAndMakeVisible(presetBox);

    inputMeterLabel.setText("INPUT", juce::dontSendNotification);
    outputMeterLabel.setText("OUTPUT", juce::dontSendNotification);
    inputMeterState.setText(juce::String::fromUTF8("\103\150\306\260\141\040\143\303\263\040\164\303\255\156\040\150\151\341\273\207\165"), juce::dontSendNotification);
    outputMeterState.setText(juce::String::fromUTF8("\103\150\306\260\141\040\143\303\263\040\164\303\255\156\040\150\151\341\273\207\165"), juce::dontSendNotification);
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
            ? juce::String::fromUTF8("\304\220\303\243\040\143\150\341\273\215\156\040\142\341\272\255\164\040\170\341\273\255\040\154\303\275\040\055\040\143\150\306\260\141\040\143\303\263\040\141\165\144\151\157\040\145\156\147\151\156\145\057\104\123\120")
            : juce::String::fromUTF8("\304\220\303\243\040\164\341\272\257\164\040\171\303\252\165\040\143\341\272\247\165\040\170\341\273\255\040\154\303\275\040\055\040\143\150\306\260\141\040\143\303\263\040\141\165\144\151\157\040\145\156\147\151\156\145\057\104\123\120"));
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
            ? juce::String::fromUTF8("\102\171\160\141\163\163\040\304\221\306\260\341\273\243\143\040\143\150\341\273\215\156\040\164\162\303\252\156\040\147\151\141\157\040\144\151\341\273\207\156\040\055\040\143\150\306\260\141\040\164\303\241\143\040\304\221\341\273\231\156\147\040\303\242\155\040\164\150\141\156\150")
            : juce::String::fromUTF8("\304\220\303\243\040\142\341\273\217\040\143\150\341\273\215\156\040\102\171\160\141\163\163"));
    };
    addAndMakeVisible(bypassButton);

    statusLabel.setText(juce::String::fromUTF8("\102\341\272\242\116\040\107\111\101\117\040\104\111\341\273\206\116\072\040\103\303\201\103\040\304\220\111\341\273\200\125\040\113\110\111\341\273\202\116\040\103\110\306\257\101\040\113\341\272\276\124\040\116\341\273\220\111\040\104\123\120"),
                       juce::dontSendNotification);
    statusLabel.setFont(juce::FontOptions(11.5f, juce::Font::bold));
    statusLabel.setColour(juce::Label::textColourId, juce::Colour(242, 183, 91));
    addAndMakeVisible(statusLabel);

    footerLabel.setText(juce::String::fromUTF8("\126\117\111\103\105\040\126\117\103\101\114\040\123\124\125\104\111\117\040\040\174\040\040\125\111\040\120\122\117\124\117\124\131\120\105\040\040\174\040\040\103\110\306\257\101\040\130\341\273\254\040\114\303\235\040\303\202\115\040\124\110\101\116\110"),
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
    auto area = getLocalBounds().reduced(16);

    auto header = area.removeFromTop(54);
    titleLabel.setBounds(header.getX() + 10, header.getY(), 420, 32);
    subtitleLabel.setBounds(header.getX() + 12, header.getY() + 31, 500, 18);

    area.removeFromTop(6);
    auto deviceArea = area.removeFromTop(70);
    deviceLabel.setBounds(deviceArea.getX() + 10, deviceArea.getY(), 180, 20);
    engineLabel.setBounds(deviceArea.getRight() - 245, deviceArea.getY(), 235, 20);

    const int half = (deviceArea.getWidth() - 36) / 2;
    inputLabel.setBounds(deviceArea.getX() + 10, deviceArea.getY() + 22, 90, 16);
    inputDeviceBox.setBounds(deviceArea.getX() + 10, deviceArea.getY() + 40, half - 10, 27);
    outputLabel.setBounds(deviceArea.getX() + half + 26, deviceArea.getY() + 22, 90, 16);
    outputDeviceBox.setBounds(deviceArea.getX() + half + 26, deviceArea.getY() + 40, half - 10, 27);

    area.removeFromTop(5);
    auto meterArea = area.removeFromTop(28);
    inputMeterLabel.setBounds(meterArea.getX() + 10, meterArea.getY(), 54, 18);
    inputMeterState.setBounds(meterArea.getX() + 66, meterArea.getY(), 220, 18);
    outputMeterLabel.setBounds(meterArea.getX() + 340, meterArea.getY(), 62, 18);
    outputMeterState.setBounds(meterArea.getX() + 405, meterArea.getY(), 220, 18);

    area.removeFromTop(7);
    auto bottom = area.removeFromBottom(55);
    const int gap = 10;
    const int panelWidth = (area.getWidth() - gap) / 2;
    const int panelHeight = (area.getHeight() - gap) / 2;

    panelBounds[0] = { area.getX(), area.getY(), panelWidth, panelHeight };
    panelBounds[1] = { area.getX() + panelWidth + gap, area.getY(), panelWidth, panelHeight };
    panelBounds[2] = { area.getX(), area.getY() + panelHeight + gap, panelWidth, panelHeight };
    panelBounds[3] = { area.getX() + panelWidth + gap, area.getY() + panelHeight + gap, panelWidth, panelHeight };

    for (int i = 0; i < 4; ++i)
    {
        auto panel = panelBounds[(size_t)i];
        sectionTitles[(size_t)i].setBounds(panel.getX() + 10, panel.getY() + 7, panel.getWidth() - 140, 22);
        sectionToggles[(size_t)i].setBounds(panel.getRight() - 132, panel.getY() + 5, 122, 24);

        if (i == 0)
        {
            auto controls = groupKnobs[0];
            const int contentY = panel.getY() + 34;
            if (!controls.empty())
            {
                controls[0]->setBounds(panel.getX() + 8, contentY, 100, panel.getHeight() - 40);
                controls[0]->setVisible(true);
                controls[0]->toFront(false);
            }

            const int sliderX = panel.getX() + 116;
            const int sliderWidth = panel.getWidth() - 126;
            micVolumeLabel.setBounds(sliderX, contentY + 2, sliderWidth, 17);
            micVolumeSlider.setBounds(sliderX, contentY + 19, sliderWidth, 30);
            outputVolumeLabel.setBounds(sliderX, contentY + 57, sliderWidth, 17);
            outputVolumeSlider.setBounds(sliderX, contentY + 74, sliderWidth, 30);
        }
        else
        {
            const auto& controls = groupKnobs[(size_t)i];
            const int count = (int)controls.size();
            const int innerWidth = panel.getWidth() - 16;
            const int cellWidth = innerWidth / juce::jmax(1, count);
            const int top = panel.getY() + 34;
            const int availableHeight = panel.getHeight() - 40;
            for (int k = 0; k < count; ++k)
            {
                const int x = panel.getX() + 8 + k * cellWidth;
                controls[(size_t)k]->setBounds(x, top, cellWidth, availableHeight);
                controls[(size_t)k]->setVisible(true);
                controls[(size_t)k]->toFront(false);
            }
        }
    }

    presetBox.setBounds(bottom.getX() + 4, bottom.getY() + 3, 165, 27);
    processButton.setBounds(bottom.getX() + 178, bottom.getY() + 2, 132, 30);
    bypassButton.setBounds(bottom.getX() + 320, bottom.getY() + 2, 142, 30);
    statusLabel.setBounds(bottom.getX() + 4, bottom.getY() + 32, bottom.getWidth() - 8, 18);
    footerLabel.setBounds(16, getHeight() - 17, getWidth() - 32, 14);
}
