/* Copyright (c) 2021, Christian Hertel */

#include "orpheus_host_connection.h"

orpheus::AudioStreamHostConnection::AudioStreamHostConnection(float sample_rate, size_t max_audio_buffer_size)
	: AudioStreamConnection(ORPHEUS_CONNECTION_TYPE_HOST, sample_rate, max_audio_buffer_size)
{}

void orpheus::AudioStreamHostConnection::add_data(const juce::AudioBuffer<float>& buffer)
{
    juce::ScopedLock lock(critical_section);

	cache_data_added.add_data(buffer);
	while (isReadyToStream() && (cache_data_added.getSize() / chunkSize) > chunksToStream) {
		streamCache(); // sends data async
	}
}

void orpheus::AudioStreamHostConnection::onStreamPacketRecieved(stream_data& packet) 
{
	//No custom behaviour
}
