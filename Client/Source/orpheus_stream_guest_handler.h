/* Copyright (c) 2021, Christian Hertel */

#pragma once

#include "orpheus_stream_handler.h"

namespace orpheus
{
	class AudioStreamGuestHandler : public AudioStreamHandler
	{
	public:
		AudioStreamGuestHandler(const juce::String& serverAddress, unsigned int serverPort);

		bool _init() override;

		void _update(const juce::AudioBuffer<float>& buffer, juce::AudioBuffer<float>& host_out, juce::AudioBuffer<float>& guest_out) override;

		void _stop();

	private:
		bool data_recieved = false;
		PipeBuffer guest_input_cache;

		std::unique_ptr<AudioStreamGuestConnection> client;
		const juce::String& serverAddr;
		unsigned int serverPort;
	};
}
