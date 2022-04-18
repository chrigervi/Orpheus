/* Copyright (c) 2021, Christian Hertel */
// TODO: Add ScopedLock to maintain thread-security

#include "orpheus_stream_handler.h"
#include "orpheus_log.h"

orpheus::AudioStreamHandler::~AudioStreamHandler()
{
	stop();
}
void orpheus::AudioStreamHandler::init(float sample_rate)
{
	ORPHLOG("Initialising . . .");

	if (!active) {
		this->sample_rate = sample_rate;
		sendActionMessage("STREAMHANDLER_INIT");
		
		// _init() does probably need long to return because of network communication and such.
		// In order to not block the JUCE message thread the function gets called on a seperate thread.
		juce::Thread::launch([this] { 
			if (_init()) 
				sendActionMessage("STREAMHANDLER_INIT_SUCCESS");
			else 
				sendActionMessage("STREAMHANDLER_INIT_FAIL");
			active = true; 
		});
	}
}
void orpheus::AudioStreamHandler::update(juce::AudioBuffer<float>& buffer, juce::AudioBuffer<float>& host_out, juce::AudioBuffer<float>& guest_out)
{
	if (isActive()) _update(buffer, host_out, guest_out);
}
void orpheus::AudioStreamHandler::stop()
{
	ORPHLOG("Stopping . . . ");

	if (active) _stop();
	active = false;
}
bool orpheus::AudioStreamHandler::isActive()
{
	return active;
}
