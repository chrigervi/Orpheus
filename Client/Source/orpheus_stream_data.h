/* Copyright (c) 2021, Christian Hertel */

/**
* This file contains the definition of orpheus::stream_data.
* stream_data represents a network audio packet for the Orpheus audio stream.
*
* Audio packets must follow a strict structure in memory, so stream_data provides functionality
* to generate such memory blocks.
*/

#include "orpheus_config.h"
#include <JuceHeader.h>

namespace orpheus {
	struct stream_data
	{
	public:
		/**
		* Structure of an Orpheus audio packet:
		* 	byte 0-7: 	id number (int)
		*		byte 8-12:	sample rate (float)
		*		byte 12-16:	size of audio data (size_t)
		*		byte 16-*:	audio data (array of floats)
		*/
		const juce::uint64 identification;
		const float sample_rate;
		const size_t data_size;
		void* data;

		/**
		* Initialises a stream_data object with seperated parameters.
		* Use this if you want to generate a memory block.
		*/
		stream_data(juce::uint64 identification, const float sample_rate, const void* data, size_t data_size);

		/**
		* Initialises a stream_data object from a memory block. (byte array)
		* Use this to convert a recieved audio packet back into the seperate parameters.
		*/
		stream_data(const byte* stream_data_pointer, size_t stream_data_pointer_size);

		/***/
		~stream_data();

		/**
		* Creates a uniform memory block in order to stream via network.
		*/
		size_t convertToDataPointer(byte*& ret_pointer);

		/***/
		static std::shared_ptr<juce::MemoryOutputStream> compressData(float* data, size_t data_size);

		/***/
		static size_t decompressData(void* dataIn, size_t dataIn_size, float*& dataOut);

		/***/
    static void setAudioCompressionQualityIndex(unsigned int qualityIndex);

		/**
		* Compresses given audio data with the Orpheus default compression settings (defined in orpheus_config.h)
		*/
		static std::shared_ptr<juce::MemoryBlock> compressAudioData(const juce::AudioSampleBuffer& audioData, float sampleRate);

		/**
		* Decompressed given audio data with the Orpheus default decompression settings (defined in orpheus_config.h)
		*/
		static std::shared_ptr<juce::AudioSampleBuffer> decompressAudioData(const void* dataIn, size_t dataIn_size, unsigned int numSamples);

	private:
        	static unsigned int _audioCompressionQualiyIndex;

		byte* stream_data_pointer = nullptr;
		size_t stream_data_size = 0;
	};
}
