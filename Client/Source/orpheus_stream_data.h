/* Copyright (c) 2021, Christian Hertel */

#include "orpheus_config.h"
#include <JuceHeader.h>

namespace orpheus {
	struct stream_data
	{
	public:
		const juce::uint64 identification;
		const float sample_rate;
		const size_t data_size;
		void* data;
		
		//stream_data(const float sample_rate, float* data, size_t data_size);
		stream_data(juce::uint64 identification, const float sample_rate, const void* data, size_t data_size);
		//init from data pointer
		stream_data(const byte* stream_data_pointer, size_t stream_data_pointer_size);
		~stream_data();

		size_t convertToDataPointer(byte*& ret_pointer);
		static std::shared_ptr<juce::MemoryOutputStream> compressData(float* data, size_t data_size);
		static size_t decompressData(void* dataIn, size_t dataIn_size, float*& dataOut);
        
        	static void setAudioCompressionQualityIndex(unsigned int qualityIndex);
		static std::shared_ptr<juce::MemoryBlock> compressAudioData(const juce::AudioSampleBuffer& audioData, float sampleRate);
		static std::shared_ptr<juce::AudioSampleBuffer> decompressAudioData(const void* dataIn, size_t dataIn_size, unsigned int numSamples);
	private:
        	static unsigned int _audioCompressionQualiyIndex;
        
		byte* stream_data_pointer = nullptr;
		size_t stream_data_size = 0;
	};
}
