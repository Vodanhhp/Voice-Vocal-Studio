
#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent()
{
    setSize(900, 640);

    // Tieu de
    titleLabel.setText("VOICE VOCAL STUDIO",
        juce::dontSendNotification);
    titleLabel.setFont(juce::FontOptions(28.0f, juce::Font::bold));
    titleLabel.setColour(juce::Label::textColourId,
        juce::Colour::fromRGB(70, 180, 245));
    addAndMakeVisible(titleLabel);

    subtitleLabel.setText("REALTIME VOICE PROCESSOR  |  VERSION 1.0",
        juce::dontSendNotification);
    subtitleLabel.setFont(juce::FontOptions(12.0f));
    subtitleLabel.setColour(juce::Label::textColourId,
        juce::Colours::lightgrey);
    addAndMakeVisible(subtitleLabel);

    // Tieu de khu vuc thiet bi
    deviceSectionLabel.setText("THIET BI AM THANH",
        juce::dontSendNotification);
    deviceSectionLabel.setFont(juce::FontOptions(17.0f, juce::Font::bold));
    deviceSectionLabel.setColour(juce::Label::textColourId,
        juce::Colours::white);
    addAndMakeVisible(deviceSectionLabel);

    inputLabel.setText("Dau vao - Microphone",
        juce::dontSendNotification);
    inputLabel.setColour(juce::Label::textColourId,
        juce::Colours::lightgrey);
    addAndMakeVisible(inputLabel);

    outputLabel.setText("Dau ra - Am thanh",
        juce::dontSendNotification);
    outputLabel.setColour(juce::Label::textColourId,
        juce::Colours::lightgrey);
    addAndMakeVisible(outputLabel);

    inputDeviceBox.addItem("Chua chon thiet bi", 1);
    inputDeviceBox.setSelectedId(1);
    addAndMakeVisible(inputDeviceBox);

    outputDeviceBox.addItem("Chua chon thiet bi", 1);
    outputDeviceBox.setSelectedId(1);
    addAndMakeVisible(outputDeviceBox);

    // Muc tin hieu
    meterLabel.setText("MUC TIN HIEU DAU VAO",
        juce::dontSendNotification);
    meterLabel.setFont(juce::FontOptions(16.0f, juce::Font::bold));
    meterLabel.setColour(juce::Label::textColourId,
        juce::Colours::white);
    addAndMakeVisible(meterLabel);

    meterStatusLabel.setText("Chua ket noi thiet bi am thanh",
        juce::dontSendNotification);
    meterStatusLabel.setColour(juce::Label::textColourId,
        juce::Colours::lightgrey);
    addAndMakeVisible(meterStatusLabel);

    // Che do
    modeSectionLabel.setText("CHE DO SU DUNG",
        juce::dontSendNotification);
    modeSectionLabel.setFont(juce::FontOptions(16.0f, juce::Font::bold));
    modeSectionLabel.setColour(juce::Label::textColourId,
        juce::Colours::white);
    addAndMakeVisible(modeSectionLabel);

    karaokeButton.setClickingTogglesState(true);
    karaokeButton.setToggleState(true, juce::dontSendNotification);
    karaokeButton.setColour(juce::TextButton::buttonColourId,
        juce::Colour::fromRGB(35, 115, 180));
    karaokeButton.setColour(juce::TextButton::textColourOffId,
        juce::Colours::white);
    karaokeButton.setColour(juce::TextButton::textColourOnId,
        juce::Colours::white);
    karaokeButton.onClick = [this]
        {
            karaokeButton.setToggleState(true, juce::dontSendNotification);
            mcButton.setToggleState(false, juce::dontSendNotification);
        };
    addAndMakeVisible(karaokeButton);

    mcButton.setClickingTogglesState(true);
    mcButton.setColour(juce::TextButton::buttonColourId,
        juce::Colour::fromRGB(65, 75, 95));
    mcButton.setColour(juce::TextButton::textColourOffId,
        juce::Colours::white);
    mcButton.setColour(juce::TextButton::textColourOnId,
        juce::Colours::white);
    mcButton.onClick = [this]
        {
            mcButton.setToggleState(true, juce::dontSendNotification);
            karaokeButton.setToggleState(false, juce::dontSendNotification);
        };
    addAndMakeVisible(mcButton);

    // Nut bat xu ly
    processingButton.setClickingTogglesState(true);
    processingButton.setColour(juce::TextButton::buttonColourId,
        juce::Colour::fromRGB(30, 145, 100));
    processingButton.setColour(juce::TextButton::textColourOffId,
        juce::Colours::white);
    processingButton.setColour(juce::TextButton::textColourOnId,
        juce::Colours::white);

    processingButton.onClick = [this]
        {
            isProcessingEnabled = processingButton.getToggleState();

            meterStatusLabel.setText(
                isProcessingEnabled
                ? "Da bat che do giao dien xu ly - chua co DSP"
                : "Da tat xu ly",
                juce::dontSendNotification);
        };
    addAndMakeVisible(processingButton);

    // Nut Bypass
    bypassButton.setClickingTogglesState(true);
    bypassButton.setColour(juce::TextButton::buttonColourId,
        juce::Colour::fromRGB(90, 95, 110));
    bypassButton.setColour(juce::TextButton::textColourOffId,
        juce::Colours::white);
    bypassButton.setColour(juce::TextButton::textColourOnId,
        juce::Colours::white);

    bypassButton.onClick = [this]
        {
            isBypassEnabled = bypassButton.getToggleState();

            meterStatusLabel.setText(
                isBypassEnabled
                ? "Bypass dang duoc chon - chua co DSP"
                : "Da tat Bypass",
                juce::dontSendNotification);
        };
    addAndMakeVisible(bypassButton);

    // Trang thai
    footerLabel.setText(
        "VOICE VOCAL STUDIO V1.0  |  Giao dien thu nghiem",
        juce::dontSendNotification);
    footerLabel.setFont(juce::FontOptions(11.0f));
    footerLabel.setColour(juce::Label::textColourId,
        juce::Colours::grey);
    addAndMakeVisible(footerLabel);
}

//==============================================================================
MainComponent::~MainComponent()
{
}

//==============================================================================
void MainComponent::paint(juce::Graphics& g)
{
    const auto background = juce::Colour::fromRGB(19, 23, 32);
    const auto panel = juce::Colour::fromRGB(31, 38, 51);

    g.fillAll(background);

    auto bounds = getLocalBounds().reduced(25);

    // Nen cac khu vuc
    auto header = bounds.removeFromTop(85);
    bounds.removeFromTop(15);

    auto deviceArea = bounds.removeFromTop(165);
    bounds.removeFromTop(15);

    auto meterArea = bounds.removeFromTop(105);
    bounds.removeFromTop(15);

    auto modeArea = bounds.removeFromTop(115);
    bounds.removeFromTop(15);

    auto controlArea = bounds.removeFromTop(85);

    g.setColour(panel);
    g.fillRoundedRectangle(deviceArea.toFloat(), 12.0f);
    g.fillRoundedRectangle(meterArea.toFloat(), 12.0f);
    g.fillRoundedRectangle(modeArea.toFloat(), 12.0f);
    g.fillRoundedRectangle(controlArea.toFloat(), 12.0f);

    // Duong nhan dien o dau trang
    g.setColour(juce::Colour::fromRGB(55, 155, 220));
    g.fillRoundedRectangle(
        juce::Rectangle<float>(
            (float)header.getX(),
            (float)header.getY() + 2.0f,
            5.0f,
            55.0f),
        2.0f);
}

//==============================================================================
void MainComponent::resized()
{
    auto bounds = getLocalBounds().reduced(25);

    // Tieu de
    auto header = bounds.removeFromTop(85);
    titleLabel.setBounds(header.getX() + 15,
        header.getY() + 5,
        header.getWidth(),
        38);

    subtitleLabel.setBounds(header.getX() + 17,
        header.getY() + 48,
        header.getWidth(),
        22);

    bounds.removeFromTop(15);

    // Thiet bi
    auto deviceArea = bounds.removeFromTop(165);

    deviceSectionLabel.setBounds(deviceArea.getX() + 18,
        deviceArea.getY() + 12,
        300, 27);

    const int columnWidth = (deviceArea.getWidth() - 54) / 2;

    inputLabel.setBounds(deviceArea.getX() + 18,
        deviceArea.getY() + 50,
        columnWidth, 24);

    inputDeviceBox.setBounds(deviceArea.getX() + 18,
        deviceArea.getY() + 80,
        columnWidth, 38);

    outputLabel.setBounds(deviceArea.getX() + 36 + columnWidth,
        deviceArea.getY() + 50,
        columnWidth, 24);

    outputDeviceBox.setBounds(deviceArea.getX() + 36 + columnWidth,
        deviceArea.getY() + 80,
        columnWidth, 38);

    bounds.removeFromTop(15);

    // Muc tin hieu
    auto meterArea = bounds.removeFromTop(105);

    meterLabel.setBounds(meterArea.getX() + 18,
        meterArea.getY() + 15,
        meterArea.getWidth() - 36, 25);

    meterStatusLabel.setBounds(meterArea.getX() + 18,
        meterArea.getY() + 50,
        meterArea.getWidth() - 36, 25);

    bounds.removeFromTop(15);

    // Che do
    auto modeArea = bounds.removeFromTop(115);

    modeSectionLabel.setBounds(modeArea.getX() + 18,
        modeArea.getY() + 10,
        300, 25);

    karaokeButton.setBounds(modeArea.getX() + 18,
        modeArea.getY() + 50,
        170, 42);

    mcButton.setBounds(modeArea.getX() + 205,
        modeArea.getY() + 50,
        190, 42);

    bounds.removeFromTop(15);

    // Dieu khien
    auto controlArea = bounds.removeFromTop(85);

    processingButton.setBounds(controlArea.getX() + 18,
        controlArea.getY() + 17,
        190, 48);

    bypassButton.setBounds(controlArea.getX() + 225,
        controlArea.getY() + 17,
        150, 48);

    // Chan trang
    footerLabel.setBounds(25,
        getHeight() - 28,
        getWidth() - 50,
        18);
}
