/* Copyright (c) 2021, Christian Hertel */

#include "orpheus_stream_handler.h"

orpheus::AudioStreamHandler::~AudioStreamHandler()
{
	stop();
}
void orpheus::AudioStreamHandler::init(float sample_rate)
{
	//ORPH_globallock();
	ORPHLOG("Initialising . . .");

	if (!active) {
		this->sample_rate = sample_rate;
		//active = true;
		//return _init();
		sendActionMessage("STREAMHANDLER_INIT");
		juce::Thread::launch([this] { 
			if (_init()) sendActionMessage("STREAMHANDLER_INIT_SUCCESS");
			else sendActionMessage("STREAMHANDLER_INIT_FAIL");
			active = true; 
		});
	}
}
void orpheus::AudioStreamHandler::update(juce::AudioBuffer<float>& buffer, juce::AudioBuffer<float>& host_out, juce::AudioBuffer<float>& guest_out)
{
	//ORPH_globallock();
	if (isActive()) _update(buffer, host_out, guest_out);
}
void orpheus::AudioStreamHandler::stop()
{
	//ORPH_globallock();
	ORPHLOG("Stopping . . . ");

	if (active) _stop();
	active = false;
}
bool orpheus::AudioStreamHandler::isActive()
{
	return active;
}
