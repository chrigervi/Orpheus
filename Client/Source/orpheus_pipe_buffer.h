/* Copyright (c) 2021, Christian Hertel */

#pragma once

#include <JuceHeader.h>
#include <list>
#include <array>
#include "orpheus_config.h"

namespace orpheus {

#define pipe_buffer_bsize 1024 * 4
#define pipe_buffer_channels 2
#define pipe_array std::array<float[pipe_buffer_bsize], pipe_buffer_channels>

	class PipeBuffer {
	public:
		void add_data(const juce::AudioBuffer<float>& buffer);
        	void add_data(const float* buffer, size_t size);
		
		std::shared_ptr<juce::AudioBuffer<float>> getNextChunk(int numSamples);
		size_t getNextChunkRaw(int numSamples, float*& data_ptr);

		static size_t convertToFloatSequence(const juce::AudioBuffer<float>& buffer, float*& data_ptr);

		size_t getSize() const;

	protected:
        	void _add_data(const float* buffer, size_t size, int sample_start = 0);
        	std::shared_ptr<juce::AudioBuffer<float>> _getNextChunk_flat(unsigned int numSamples);
        	std::shared_ptr<juce::AudioBuffer<float>> _getNextChunk(int numSamples);
        
		std::list<pipe_array> data;
		int chunk_write_position = 0;
		int chunk_read_position = 0;

		juce::CriticalSection lock;
	};
}
