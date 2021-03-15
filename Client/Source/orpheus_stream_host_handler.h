/* Copyright (c) 2021, Christian Hertel */

#pragma once

#include "orpheus_stream_handler.h"

namespace orpheus {
	class AudioStreamHostHandler : public AudioStreamHandler
	{
	public:
		AudioStreamHostHandler(const juce::String& serverAddress, unsigned int serverPort);

		bool _init() override;

		void _update(const juce::AudioBuffer<float>& buffer, juce::AudioBuffer<float>& host_out, juce::AudioBuffer<float>& guest_out) override;

		void _stop() override;

	private:
		juce::String serverAddr;
		unsigned int serverPort;

		orpheus::PipeBuffer host_cache;
		std::unique_ptr<AudioStreamHostConnection> host;
	};
}
