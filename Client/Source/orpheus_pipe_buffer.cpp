/* Copyright (c) 2021, Christian Hertel */

#include "orpheus_pipe_buffer.h"

#include "orpheus_log.h"

void orpheus::PipeBuffer::add_data(const juce::AudioBuffer<float>& buffer)
{
	float* data_pntr = nullptr;
	size_t size = convertToFloatSequence(buffer, data_pntr);
	add_data(data_pntr, size);

	delete[] data_pntr;
}

void orpheus::PipeBuffer::add_data(const float *buffer, size_t size)
{
    juce::ScopedLock lockguard(lock);
    _add_data(buffer, size, 0);
}

void orpheus::PipeBuffer::_add_data(const float* buffer, size_t size, int sample_start)
{
	if (chunk_write_position == 0 || data.size() == 0) {
		data.resize(data.size() + 1);
	}

	int chsize = size / pipe_buffer_channels;
	int chunk_rem = pipe_buffer_bsize - chunk_write_position;
	int samples_to_add = (chunk_rem > chsize - sample_start) ? chsize - sample_start : chunk_rem;
	//add samples to chunk
	auto element = data.back();
	for (int channel = 0; channel < pipe_buffer_channels; channel++) {
		memcpy(element[channel] + chunk_write_position, buffer + (channel * chsize) + sample_start, samples_to_add * sizeof(float));
	}
	data.pop_back();
	data.push_back(element);

	chunk_write_position += samples_to_add;
	chunk_write_position %= pipe_buffer_bsize;

	int samples_rem = chsize - samples_to_add - sample_start;
	if (samples_rem > 0) {
		chunk_write_position = 0;
		_add_data(buffer, size, samples_to_add + sample_start);
	}
}

std::shared_ptr<juce::AudioBuffer<float>> orpheus::PipeBuffer::getNextChunk(int numSamples)
{
    juce::ScopedLock lockguard(lock);
    return _getNextChunk_flat(numSamples);
}

std::shared_ptr<juce::AudioBuffer<float>> orpheus::PipeBuffer::_getNextChunk(int numSamples)
{
    if (data.empty() || getSize() < numSamples) {
		return nullptr;
	}

	std::shared_ptr<juce::AudioBuffer<float>> result = std::make_shared<juce::AudioBuffer<float>>();
	result->setSize(pipe_buffer_channels, numSamples, false, true, true);

	if (numSamples + chunk_read_position < pipe_buffer_bsize)
	{
		auto data_e = data.front();
		for (int i = 0; i < pipe_buffer_channels; i++)
		{
			result->copyFrom(i, 0, data_e[i] + chunk_read_position, numSamples);
		}

		chunk_read_position += numSamples;
	}
	else
	{
		auto data_e = data.front();
		auto remaining = pipe_buffer_bsize - chunk_read_position;
		for (int i = 0; i < pipe_buffer_channels; i++)
		{
			result->copyFrom(i, 0, data_e[i] + chunk_read_position, remaining);
		}

		data.pop_front();
		chunk_read_position = 0;
        
		auto next_chunk = this->_getNextChunk(numSamples - remaining);
		if (next_chunk == nullptr) 
			next_chunk = std::make_shared<juce::AudioBuffer<float>>(pipe_buffer_channels, numSamples - remaining);

		for (int i = 0; i < pipe_buffer_channels; i++)
		{
			result->copyFrom(i, remaining, next_chunk->getReadPointer(i), numSamples - remaining);
		}
	}

	return result;
}

std::shared_ptr<juce::AudioBuffer<float>> orpheus::PipeBuffer::_getNextChunk_flat(unsigned int numSamples)
{
    if (data.empty() || getSize() < numSamples)
    {
        return nullptr;
    }

    std::shared_ptr<juce::AudioBuffer<float>> result = std::make_shared<juce::AudioBuffer<float>>();
    result->setSize(pipe_buffer_channels, numSamples, false, true, true);
    
    unsigned int samplesToWrite = numSamples;
    
    while (samplesToWrite > 0)
    {
        const unsigned int currentBufferSize = pipe_buffer_bsize - chunk_read_position;
        const unsigned int samplesWritten = numSamples - samplesToWrite;
        
        if (samplesToWrite <= currentBufferSize)
        {
            auto data_e = data.front();
            for (int i = 0; i < pipe_buffer_channels; i++)
            {
                result->copyFrom(i, samplesWritten, data_e[i] + chunk_read_position, samplesToWrite);
            }
            
            chunk_read_position += samplesToWrite;
            samplesToWrite = 0;
        }
        else
        {
            auto data_e = data.front();
            auto remaining = pipe_buffer_bsize - chunk_read_position;
            for (int i = 0; i < pipe_buffer_channels; i++)
            {
                result->copyFrom(i, samplesWritten, data_e[i] + chunk_read_position, remaining);
            }
            
            data.pop_front();
            chunk_read_position = 0;
            samplesToWrite -= remaining;
        }
    }

    return result;
}

size_t orpheus::PipeBuffer::getNextChunkRaw(int numSamples, float*& data_ptr)
{
	auto chunk = getNextChunk(numSamples);
	if (chunk == nullptr) {
		data_ptr = nullptr;
		return 0;
	}

	size_t chunk_raw_size = convertToFloatSequence(*chunk, data_ptr);

	return chunk_raw_size;
}

size_t orpheus::PipeBuffer::convertToFloatSequence(const juce::AudioBuffer<float>& buffer, float*& data_ptr)
{
	size_t data_size = buffer.getNumSamples() * buffer.getNumChannels();
	float* data = new float[data_size];
	for (int i = 0; i < buffer.getNumChannels(); i++)
	{
		memcpy((data + i * buffer.getNumSamples()), buffer.getReadPointer(i), buffer.getNumSamples() * sizeof(float));
	}

	data_ptr = data;

	return data_size;
}

size_t orpheus::PipeBuffer::getSize() const
{
	size_t size = pipe_buffer_bsize * (data.size() - ((chunk_write_position != 0) ? 1 : 0)) + chunk_write_position - chunk_read_position;
	return size;
}
