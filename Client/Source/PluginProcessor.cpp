/* Copyright (c) 2021, Christian Hertel */

#include "PluginProcessor.h"
#include "PluginEditor.h"

#include "orpheus_stream_host_handler.h"
#include "orpheus_stream_guest_handler.h"

#include <chrono>
#include <string>
#include "orpheus_DataTracker.h"

//==============================================================================
OrpheusAudioProcessor::OrpheusAudioProcessor() :
#ifndef JucePlugin_PreferredChannelConfigurations
	AudioProcessor(BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
		.withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
		.withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
	)
#endif
{
	//INITIALSATION
	orpheus::orpheus_init();
    orpheus::DataTracker::setGlobalTracker(&_tracker);
}

OrpheusAudioProcessor::~OrpheusAudioProcessor()
{
	stopNet();
    orpheus::DataTracker::setGlobalTracker(nullptr);
}

//==============================================================================
const juce::String OrpheusAudioProcessor::getName() const
{
	return JucePlugin_Name;
}
bool OrpheusAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
	return true;
   #else
	return false;
   #endif
}
bool OrpheusAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
	return true;
   #else
	return false;
   #endif
}
bool OrpheusAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
	return true;
   #else
	return false;
   #endif
}
double OrpheusAudioProcessor::getTailLengthSeconds() const
{
	return 0.0;
}
int OrpheusAudioProcessor::getNumPrograms()
{
	return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
				// so this should be at least 1, even if you're not really implementing programs.
}
int OrpheusAudioProcessor::getCurrentProgram()
{
	return 0;
}
void OrpheusAudioProcessor::setCurrentProgram (int index)
{
}
const juce::String OrpheusAudioProcessor::getProgramName (int index)
{
	return {};
}
void OrpheusAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void OrpheusAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
	this->sample_rate = sampleRate;
	ORPH_TRACKDATA("Sample-Rate", sample_rate);
}
void OrpheusAudioProcessor::releaseResources()
{
	// When playback stops, you can use this as an opportunity to free up any
	// spare memory, etc.
}
#ifndef JucePlugin_PreferredChannelConfigurations
bool OrpheusAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
	juce::ignoreUnused (layouts);
	return true;
  #else
	// This is the place where you check if the layout is supported.
	// In this template code we only support mono or stereo.
	/*if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
	 && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
		return false;*/
	auto numIns = layouts.getMainInputChannels();
	auto numOuts = layouts.getMainOutputChannels();

	// allow any configuration where there are at least 2 channels
	// and the number of inputs equals the number of outputs
	return (numIns == 4 && numOuts == 4);

	// This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
	if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
		return false;
   #endif

	return true;
  #endif
}
#endif


void OrpheusAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
	juce::ScopedLock lock(critical_section);

	juce::ScopedNoDenormals noDenormals;
	auto totalNumInputChannels  = getTotalNumInputChannels();
	auto totalNumOutputChannels = getTotalNumOutputChannels();

	for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
		buffer.clear (i, 0, buffer.getNumSamples());

	if (isStreamActive())
	{
		//Init output buffers
		juce::AudioBuffer<float> host_out(2, buffer.getNumSamples());
		juce::AudioBuffer<float> guest_out(2, buffer.getNumSamples());

		//process audio
		stream_handler->update(buffer, host_out, guest_out);

		//add host buffer to output
		buffer = host_out;
		buffer.applyGain(host_gain_factor);
		//add guest buffer to output
		guest_out.applyGain(guest_gain_factor);
		for (int ch = 0; ch < guest_out.getNumChannels(); ch++) {
			buffer.addFrom(ch, 0, guest_out.getReadPointer(ch), guest_out.getNumSamples(), guest_gain_factor);
		}

		//Gain calculations for the editor's volume-meter
		host_gain = host_out.getRMSLevel(0, 0, buffer.getNumSamples());
		guest_gain = guest_out.getRMSLevel(0, 0, buffer.getNumSamples());
		sendActionMessage("PROCESS_BLOCK");
	}

	ORPH_TRACKDATA("Buffer-Size", buffer.getNumSamples());
}

//==============================================================================
bool OrpheusAudioProcessor::hasEditor() const
{
	return true; // (change this to false if you choose to not supply an editor)
}
juce::AudioProcessorEditor* OrpheusAudioProcessor::createEditor()
{
	return new OrpheusAudioProcessorEditor (*this);
}
//==============================================================================
void OrpheusAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
	// You should use this method to store your parameters in the memory block.
	// You could do that either as raw data, or use the XML or ValueTree classes
	// as intermediaries to make it easy to save and load complex data.
}
void OrpheusAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
	// You should use this method to restore your parameters from this memory block,
	// whose contents will have been created by the getStateInformation() call.
}

void OrpheusAudioProcessor::setIsServerMode(bool mode)
{
	this->is_server_mode = mode;
}
bool OrpheusAudioProcessor::isServerMode()
{
	return is_server_mode;
}
void OrpheusAudioProcessor::setRawServerIP(juce::String& rawIP)
{
    this->rawIP = rawIP;
}
bool OrpheusAudioProcessor::readServerAddressInformation()
{
    juce::StringArray tokens;
    tokens.addTokens(rawIP, ":", "\"");
    
    if (tokens.size() != 2)
    {
        orpheus::dialogError("The Server-Address is invalid!\n(IPADRESS:PORT)");
        serverIPAddress = "";
        serverPort = 0;
        
        return false;
    }
    
    serverIPAddress = tokens.operator[](0);
    
    try {
        serverPort = std::stoi(tokens[1].toRawUTF8());
    } catch (std::exception e) {
        serverPort = 0;
        orpheus::dialogError("The Server-Port is invalid!");
        
        return false;
    }
    
    return true;
}

void OrpheusAudioProcessor::initNet()
{
	juce::ScopedLock lock(critical_section);
    if (!readServerAddressInformation()) return; //update serverIPAddress, serverPort

	stopNet();
	if (is_server_mode)
		stream_handler = std::make_unique<orpheus::AudioStreamHostHandler>(serverIPAddress, serverPort);
	else
		stream_handler = std::make_unique<orpheus::AudioStreamGuestHandler>(serverIPAddress, serverPort);

	stream_handler->addActionListener(this);
	stream_handler->init(this->sample_rate);
}
void OrpheusAudioProcessor::stopNet()
{
	juce::ScopedLock lock(critical_section);

	if (stream_handler.get() != nullptr) {
		stream_handler->stop();
		stream_handler.reset();
	}
}
bool OrpheusAudioProcessor::isStreamActive()
{
	return stream_handler == nullptr? false : stream_handler->isActive();
}

float OrpheusAudioProcessor::getCurrentHostGain()
{
	return host_gain;
}
float OrpheusAudioProcessor::getCurrentGuestGain()
{
	return guest_gain;
}
void OrpheusAudioProcessor::setHostGainFactor(float gain_factor)
{
	host_gain_factor = gain_factor;
}
void OrpheusAudioProcessor::setGuestGainFactor(float gain_factor)
{
	guest_gain_factor = gain_factor;
}

void OrpheusAudioProcessor::actionListenerCallback(const juce::String& msg)
{
	sendActionMessage(msg);
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
	return new OrpheusAudioProcessor();
}
