/* Copyright (c) 2021, Christian Hertel */

#include "orpheus_stream_host_handler.h"
#include "orpheus_host_connection.h"
#include "sandbox.h"
#include "orpheus_DataTracker.h"

orpheus::AudioStreamHostHandler::AudioStreamHostHandler(const juce::String& serverAddress, unsigned int serverPort)
	: serverAddr(serverAddress), serverPort(serverPort)
{}
bool orpheus::AudioStreamHostHandler::_init()
{
	if (host.get() == nullptr)
	{
		ORPHLOG("Connecting to " + std::string(serverAddr.toRawUTF8()) + " on port " + std::to_string(serverPort));

		host = std::make_unique<::orpheus::AudioStreamHostConnection>(this->sample_rate);
		bool success = host->connect(serverAddr, serverPort);
		return success;
	}

	return false;
}
void orpheus::AudioStreamHostHandler::_update(const juce::AudioBuffer<float>& buffer, juce::AudioBuffer<float>& host_out, juce::AudioBuffer<float>& guest_out)
{
	const auto buffer_size = buffer.getNumSamples();

	//default: no host out, no guest out
	host_out.clear();
	guest_out.clear();

	if (host.get() != nullptr && host->isConnected() == false)
	{
		stop();
		return;
	}

	if (host.get() != nullptr && host->isReadyToStream()) {
		// forward audio data to the guest
		host->add_data(buffer);
		// cache data
		host_cache.add_data(buffer);

		//std::shared_ptr<juce::AudioBuffer<float>> guest_data = nullptr;
		//guest_data = (host->getRecievedData().getSize() >= ORPHEUS_RECDATA_BUFFERS) ? host->getNextSamples(buffer_size) : nullptr;
		auto guest_data = host->getNextSamples(buffer_size);

		if (guest_data.get() != nullptr) {
			host_out = *host_cache.getNextChunk(buffer_size);
			guest_out = *guest_data.get();

			ORPH_TRACKDATA("Delay (samples)", host_cache.getSize());
			ORPH_TRACKDATA("Delay-Time", host_cache.getSize() * (1.0f / sample_rate));
		}
	}
}
void orpheus::AudioStreamHostHandler::_stop()
{
	if (host->isConnected()) host->disconnect();
	host.reset();
}
