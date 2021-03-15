/* Copyright (c) 2021, Christian Hertel */

#pragma once

#include "orpheus_connection.h"

namespace orpheus
{
	class AudioStreamGuestConnection : public AudioStreamConnection
	{
	public:
		AudioStreamGuestConnection(float sample_rate, size_t max_audio_buffer_size = ORPHEUS_AUDIOSTREAM_BSIZE);

		void add_data(const juce::AudioBuffer<float>& buffer) override;

	private:
		void onStreamPacketRecieved(stream_data& packet) override;
	};
}