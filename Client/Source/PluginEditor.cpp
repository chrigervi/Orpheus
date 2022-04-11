/* Copyright (c) 2021, Christian Hertel */

/**
* This file contains the implementation of OrpheusAudioProcessorEditor.
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

#include "orpheus_config.h"
#include "sandbox.h"
#include "orpheus_DataTracker.h"

//==============================================================================
OrpheusAudioProcessorEditor::OrpheusAudioProcessorEditor(OrpheusAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.

    ORPHLOG("Load UI");

    _dataTracker = new orpheus::DataTrackerTable(orpheus::DataTracker::getGlobalTracker());

    setSize(ORPHEUS_EDITOR_WIDTH, (1.0f / ORPHEUS_EDITOR_ASPECT_RATIO) * ORPHEUS_EDITOR_WIDTH);
    audioProcessor.addActionListener(this);

    addAndMakeVisible(_contentContainer);
    _contentContainer.addAndMakeVisible(panelButtons);
    _contentContainer.addAndMakeVisible(panelGeneralInfo);
    _contentContainer.addAndMakeVisible(panelHost);

    // init general control buttons
    buttonHost.onClick = [this] { audioProcessor.setIsServerMode(true); /*addAndMakeVisible(panelHost); removeChildComponent(&panelGuest);*/ };
    buttonHost.setClickingTogglesState(true);
    buttonHost.setToggleState(true, juce::NotificationType::sendNotificationSync);
    buttonHost.setRadioGroupId(1);
    panelButtons.addAndMakeVisible(buttonHost);

    buttonGuest.onClick = [this] { audioProcessor.setIsServerMode(false); /*addAndMakeVisible(panelGuest); removeChildComponent(&panelHost);*/ };
    buttonGuest.setClickingTogglesState(true);
    buttonGuest.setRadioGroupId(1);
    panelButtons.addAndMakeVisible(buttonGuest);

    buttonStart.onClick = [this] {
        on_net_start();
    };
    panelButtons.addAndMakeVisible(buttonStart);

    buttonStop.setEnabled(false);
    buttonStop.onClick = [this] {
        on_net_stop();
    };
    panelButtons.addAndMakeVisible(buttonStop);

    // init general information panel
    panelGeneralInfo.addAndMakeVisible(label_ip);
    panelGeneralInfo.addAndMakeVisible(text_ip);

    label_ip.setText("Server-IP:", juce::NotificationType::sendNotificationSync);
    //label_ip.setColour(juce::Label::ColourIds::outlineColourId, ORPHEUS_BG_DARK_COLOR);
    text_ip.setText("localhost:17995");
    text_ip.onTextChange = [this] {
        auto ip = text_ip.getText();
        audioProcessor.setRawServerIP(ip);
    };
    text_ip.onTextChange(); //assign init values to processor
    text_ip.setMultiLine(false, false);
    text_ip.setJustification(juce::Justification::centredLeft);
    text_ip.setColour(juce::TextEditor::ColourIds::outlineColourId, juce::Colours::transparentWhite);
    text_ip.setColour(juce::TextEditor::ColourIds::focusedOutlineColourId, juce::Colours::transparentWhite);
    text_ip.setColour(juce::TextEditor::ColourIds::backgroundColourId, ORPHEUS_BGSUB_COLOR);


    // panelHost
    panelHost.setViewedComponent(_dataTracker, false);
    panelHost.setWantsKeyboardFocus(false);
    panelHost.setScrollBarsShown(true, false, true, true);
    panelHost.setScrollBarThickness(10);

    _contentContainer.addAndMakeVisible(_hostGain);
    _contentContainer.addAndMakeVisible(_guestGain);
}

OrpheusAudioProcessorEditor::~OrpheusAudioProcessorEditor()
{
    delete _dataTracker;
}

//==============================================================================
void OrpheusAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    //g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
    g.fillAll(ORPHEUS_BGSUB_COLOR);

    if (_showLoadingScreen)
    {
        //setVisible(false);
        //addAndMakeVisible(_loadingScreen);

        _contentContainer.setVisible(false);
        _loadingScreen.setBounds(0, 0, getWidth(), getHeight());

        auto b = getBounds();
        b.reduce(getWidth() / 2.5f, getHeight() / 2.5f);
        getLookAndFeel().drawSpinningWaitAnimation(g, ORPHEUS_BG_DARK_COLOR, b.getX(), b.getY(), b.getWidth(), b.getHeight());

        repaint();
        return;
    }
    else
    {
        _contentContainer.setVisible(true);
        removeChildComponent(getIndexOfChildComponent(&_loadingScreen));
    }

    g.setColour(ORPHEUS_BG_COLOR);
    g.fillRoundedRectangle(panelButtons.getBounds().toFloat(), 4.0f);
    g.fillRoundedRectangle(panelGeneralInfo.getBounds().toFloat(), 4.0f);
    g.fillRoundedRectangle(panelHost.getBounds().toFloat(), 4.0f);

    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
}

void OrpheusAudioProcessorEditor::resized()
{
    setBounds(getX(), getY(), getWidth(), ORPHEUS_EDITOR_ASPECT_RATIO * (float)getWidth());
    _contentContainer.setBounds(0, 0, getWidth(), getHeight());

#define relw(w) proportionOfWidth(w)
#define relh(h) proportionOfHeight(h)
    float marginX = 0.02f;
    float marginY = 0.02f;
    const int panelWidth = relw(1 - 2 * marginX);

    panelButtons.setBounds(relw(marginX), relh(marginY), panelWidth, relh(0.25));
    panelGeneralInfo.setBounds(relw(marginX), panelButtons.getBottom() + relh(marginY), panelWidth, relh(0.1));
    juce::Rectangle hostGuestPanelBounds(relw(marginX), panelGeneralInfo.getBottom() + relh(marginY), panelWidth, relh(0.35));
    panelHost.setBounds(hostGuestPanelBounds);

    // BUTTONS
#define relw(w) panelButtons.proportionOfWidth(w)
#define relh(h) panelButtons.proportionOfHeight(h)
    marginY = 0.1;
    const int buttonHeight = relh(0.2);
    const int hostGuestButtonWidth = relw(0.5 - 1.5* marginX);
    const int startStopButtonWidth = relw(1 - 2 * marginX);
    buttonHost.setBounds(relw(marginX), relh(marginY), hostGuestButtonWidth, buttonHeight);
    buttonGuest.setBounds(buttonHost.getRight() + relw(marginX), relh(marginY), hostGuestButtonWidth, buttonHeight);
    buttonStart.setBounds(relw(marginX), buttonHost.getBottom() + relh(marginY), startStopButtonWidth, buttonHeight);
    buttonStop.setBounds(relw(marginX), buttonStart.getBottom() + relh(marginY), startStopButtonWidth, buttonHeight);

    // GENERAL INFO
#define relw(w) panelGeneralInfo.proportionOfWidth(w)
#define relh(h) panelGeneralInfo.proportionOfHeight(h)
    int labelWidth = relw(0.25f);
    int labelHeight = relh(0.4);
    marginY = (1 - 2*0.3) / 2.0f;
    label_ip.setBounds(relw(marginX), relh(marginY), labelWidth, labelHeight);
    text_ip.setBounds(label_ip.getRight(), relh(marginY), relw(0.4), labelHeight);

    labelPort.setBounds(relw(marginX), label_ip.getBottom() + relh(marginY), labelWidth, labelHeight);
    textPort.setBounds(labelPort.getRight(), label_ip.getBottom() + relh(marginY), relw(0.4), labelHeight);

    // PANEL HOST
    _dataTracker->setSize(panelHost.getWidth(), 0); //DataTrackerTable will update its size accordingly

#define relw(w) proportionOfWidth(w)
#define relh(h) proportionOfHeight(h)
    // GAIN METERS

    _hostGain.setBounds(relw(marginX), panelHost.getBottom() + relw(marginX), 200, 90);
    _guestGain.setBounds(relw(0.5), panelHost.getBottom() + relw(marginX), 200, 90);

#undef relw
#undef relh
}

void OrpheusAudioProcessorEditor::actionListenerCallback(const juce::String& msg)
{
    if (msg.compare("PROCESS_BLOCK") == 0)
    {
        _hostGain.inputGain(audioProcessor.getCurrentHostGain());
        _guestGain.inputGain(audioProcessor.getCurrentGuestGain());

        audioProcessor.setHostGainFactor(_hostGain.getGainSliderValue());
        audioProcessor.setGuestGainFactor(_guestGain.getGainSliderValue());

        if (audioProcessor.isStreamActive() == false) on_net_stop();
    }
    else if (msg.compare("STREAMHANDLER_INIT") == 0)
    {
        _showLoadingScreen = true;
        repaint();
    }
    else if (msg.compare("STREAMHANDLER_INIT_SUCCESS") == 0)
    {
        _showLoadingScreen = false;

        buttonHost.setEnabled(false);
        buttonGuest.setEnabled(false);
        buttonStart.setEnabled(false);
        buttonStop.setEnabled(true);

        text_ip.setReadOnly(true);

        orpheus::dialogText("Orpheus", "Successfully initialised!");
    }
    else if (msg.compare("STREAMHANDLER_INIT_FAIL") == 0)
    {
        _showLoadingScreen = false;

        orpheus::println("Error occured while initialising the network-component!", "AudioEditor");
        if (audioProcessor.isServerMode())
            orpheus::dialogError("An error occured while initialising the host!");
        else
            orpheus::dialogError("An error occured while connecting to host!");
    }
}

void OrpheusAudioProcessorEditor::on_net_start()
{
    audioProcessor.initNet();
}

void OrpheusAudioProcessorEditor::on_net_stop()
{
    audioProcessor.stopNet();
    buttonHost.setEnabled(true);
    buttonGuest.setEnabled(true);
    buttonStart.setEnabled(true);
    buttonStop.setEnabled(false);

    text_ip.setReadOnly(false);
}
