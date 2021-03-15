#pragma once

#include "orpheus_connection.h"
#include "orpheus_guest_connection.h"
#include "orpheus_host_connection.h"

namespace orpheus {

#ifdef ORPHEUS_INLCUDE_HOSTSERVER
	class AudioStreamServer : public juce::InterprocessConnectionServer
	{
	public:

		AudioStreamServer(float sample_rate, size_t max_audio_buffer_size = ORPHEUS_AUDIOSTREAM_BSIZE);
		~AudioStreamServer();

		void add_data(const juce::AudioBuffer<float>& buffer);

		AudioStreamHostConnection* getConnection(unsigned int index);

	protected:
		juce::InterprocessConnection* createConnectionObject() override;

	private:
		size_t max_buffer_size;
		unsigned int sample_write_position = 0;
		float sample_rate;

		juce::AudioBuffer<float> audio_buffer_send;
		std::vector<AudioStreamHostConnection*> connections;
	};
#endif
}