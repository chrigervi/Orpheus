/* Copyright (c) 2021, Christian Hertel */

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

#include "orpheus_GainMeter.h"
#include "orpheus_logger_textfield.h"
#include "orpheus_GuiComponents.h"
#include "orpheus_ConsoleWindow.h"
#include "orpheus_DataTrackerTable.h"

//==============================================================================
/**
*/
class OrpheusAudioProcessorEditor : public juce::AudioProcessorEditor, public juce::ActionListener
{
public:
    OrpheusAudioProcessorEditor(OrpheusAudioProcessor&);
    ~OrpheusAudioProcessorEditor() override;

    //==============================================================================
    void paint(juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    OrpheusAudioProcessor& audioProcessor;

    bool _showLoadingScreen = false;
    juce::Component _loadingScreen;
    juce::Component _contentContainer;

    std::unique_ptr<juce::ResizableCornerComponent> resizer;

    juce::Component panelButtons;
    juce::Component panelGeneralInfo;
    juce::Viewport panelHost;

    orpheus::ConsoleWindow _console;
    orpheus::DataTrackerTable* _dataTracker;

    orpheus::Button buttonHost{ "Host" };
    orpheus::Button buttonGuest{ "Guest" };
    orpheus::Button buttonStart{ "Start" };
    orpheus::Button buttonStop{ "Stop" };

    juce::Slider slider;

    orpheus::GainMeter _hostGain{"Host-Out"};
    orpheus::GainMeter _guestGain{"Guest-Out"};

    juce::Label label_ip;
    juce::TextEditor text_ip;
    juce::Label labelPort;
    juce::TextEditor textPort;

    void actionListenerCallback(const juce::String&) override;

    void on_net_start();
    void on_net_stop();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OrpheusAudioProcessorEditor)
};
