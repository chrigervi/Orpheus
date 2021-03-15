/* Copyright (c) 2021, Christian Hertel */

#include "orpheus_guest_connection.h"

//========= Audio Stream Client Connection =========
orpheus::AudioStreamGuestConnection::AudioStreamGuestConnection(float sample_rate, size_t max_audio_buffer_size)
	: AudioStreamConnection(ORPHEUS_CONNECTION_TYPE_GUEST, sample_rate, max_audio_buffer_size)
{}

void orpheus::AudioStreamGuestConnection::add_data(const juce::AudioBuffer<float>& buffer)
{
	juce::ScopedLock lock(critical_section);
	cache_data_added.add_data(buffer);
}

void orpheus::AudioStreamGuestConnection::onStreamPacketRecieved(stream_data& packet)
{
	//thread safe
	juce::ScopedLock lock(critical_section);

	//SYNCHRONISATION: Guest sends data ONLY in response to a host's packet.
	streamCache();
}