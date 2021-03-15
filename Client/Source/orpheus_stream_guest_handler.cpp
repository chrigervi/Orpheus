/* Copyright (c) 2021, Christian Hertel */

#include "orpheus_stream_guest_handler.h"

orpheus::AudioStreamGuestHandler::AudioStreamGuestHandler(const juce::String& serverAddress, unsigned int serverPort)
	: serverAddr(serverAddress), serverPort(serverPort)
{}
bool orpheus::AudioStreamGuestHandler::_init()
{
	if (client.get() == nullptr)
	{
		ORPHLOG("Connecting to " + std::string(serverAddr.toRawUTF8()) + " on port " + std::to_string(serverPort));
        //auto str = "" + std::string(serverAddr.toRawUTF8());
		client = std::make_unique<orpheus::AudioStreamGuestConnection>(this->sample_rate);
		bool success = client->connect(serverAddr, serverPort);
		return success;
	}

	return false;
}
void orpheus::AudioStreamGuestHandler::_update(const juce::AudioBuffer<float>& buffer, juce::AudioBuffer<float>& host_out, juce::AudioBuffer<float>& guest_out)
{
	const auto buffer_size = buffer.getNumSamples();

	//default: pass host out, no guest out
	host_out.clear();
	guest_out.clear();

	if (client.get() != nullptr && client->isConnected() == false)
	{
		stop();
		return;
	}

	//auto databuff = (client->getRecievedData().getSize() >= ORPHEUS_RECDATA_BUFFERS) ? client->getNextSamples(buffer_size) : std::shared_ptr<juce::AudioBuffer<float>>();
	auto databuff = client->getNextSamples(buffer_size);
	if (databuff.get() != nullptr && client->isReadyToStream())
	{
		client->add_data(buffer);
		guest_out = buffer;

		// Wait one iteration
		if (data_recieved) {
			host_out = *databuff.get();
		}
		else {
			data_recieved = true;
		}
	}
}
void orpheus::AudioStreamGuestHandler::_stop()
{
	if (client->isConnected()) client->disconnect();
	client.reset();
}
