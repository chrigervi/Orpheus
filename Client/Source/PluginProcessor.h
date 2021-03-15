/* Copyright (c) 2021, Christian Hertel */

#pragma once

#include <JuceHeader.h>
#include <thread>
#include "orpheus_net.h"
#include "orpheus_stream_handler.h"
#include "orpheus_DataTracker.h"

//==============================================================================
/**
*/
class OrpheusAudioProcessor : public juce::AudioProcessor, public juce::ActionBroadcaster, public juce::ActionListener
{
public:
    //==============================================================================
    OrpheusAudioProcessor();
    ~OrpheusAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;


    void setIsServerMode(bool mode);
    bool isServerMode();

    bool readServerAddressInformation();
    void setRawServerIP(juce::String& rawIP);
    
    void initNet();
    void stopNet();
    bool isStreamActive();

    float getCurrentHostGain();
    float getCurrentGuestGain();

    void setHostGainFactor(float gain_factor);
    void setGuestGainFactor(float gain_factor);

private:
    double sample_rate;
    unsigned int sync_offset_samples;

    //DataTracker
    orpheus::DataTracker _tracker;
    
    //SERVER STUFF
    bool is_server_mode = true;
    juce::String rawIP;
    juce::String serverIPAddress;
    unsigned int serverPort;
    std::unique_ptr<orpheus::AudioStreamHandler> stream_handler;
    juce::CriticalSection critical_section;

    //
    float host_gain = 0;
    float guest_gain = 0;
    float host_gain_factor = 1.0f;
    float guest_gain_factor = 1.0f;

    //
    void actionListenerCallback(const juce::String&);

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OrpheusAudioProcessor)
};
