/* Copyright (c) 2021, Christian Hertel */

#pragma once

#include <JuceHeader.h>

#define delay_buffer_ramp_db 0.9f

namespace orpheus {

	class DelayBuffer {
	public:
		DelayBuffer(unsigned int delay_samples)
			: delay_samples(delay_samples), delay_buffer(2, delay_samples), copy_buffer(2, delay_samples)
		{}

		std::shared_ptr<juce::AudioBuffer<float>> update_buffer(unsigned int ret_buffer_size, juce::AudioBuffer<float>& audio_input)
		{
			auto delay_data = std::make_shared<juce::AudioBuffer<float>>(2, ret_buffer_size);
			const int delay_buffer_length = delay_buffer.getNumSamples();

			delay_position %= delay_samples;

			int read_position = (delay_buffer_length + delay_position - delay_samples) % delay_samples;

			for (int channel = 0; channel < delay_buffer.getNumChannels(); channel++)
			{
				float* input_channel_data = audio_input.getWritePointer(channel);
				float* output_channel_data = delay_data->getWritePointer(channel);
				float* delay_channel_data = delay_buffer.getWritePointer(channel);

				copy_buffer.copyFrom(channel, 0, delay_channel_data, delay_samples);
				float* copy_data = copy_buffer.getWritePointer(channel);

				if (delay_samples < ret_buffer_size)
				{
					/*copy_buffer.copyFrom(channel, 0, delay_channel_data, delay_samples);

					delay_data->copyFromWithRamp(channel, 0, delay_channel_data, delay_samples, delay_buffer_ramp_db, delay_buffer_ramp_db);
					delay_data->copyFrom(channel, delay_samples, audio_input, channel, 0, ret_buffer_size - delay_samples);

					delay_buffer.copyFrom(channel, ret_buffer_size - delay_samples, input_channel_data, delay_samples);*/

					delay_data->copyFrom(channel, 0, delay_channel_data, delay_samples);
					delay_data->copyFrom(channel, delay_samples, audio_input, channel, 0, ret_buffer_size - delay_samples);

					delay_buffer.copyFrom(channel, 0, audio_input, channel, ret_buffer_size - delay_samples, delay_samples);
				}
				else
				{
					//fill delay buffer
					if (delay_samples > ret_buffer_size + delay_position)
					{
						delay_buffer.copyFromWithRamp(channel, delay_position, input_channel_data, ret_buffer_size, delay_buffer_ramp_db, delay_buffer_ramp_db);
					}
					else
					{
						const int buffer_remaining = delay_samples - delay_position;
						delay_buffer.copyFromWithRamp(channel, delay_position, input_channel_data, buffer_remaining, delay_buffer_ramp_db, delay_buffer_ramp_db);
						delay_buffer.copyFromWithRamp(channel, 0, input_channel_data, ret_buffer_size - buffer_remaining, delay_buffer_ramp_db, delay_buffer_ramp_db);
					}

					//fill return buffer (delay_data)
					if (delay_samples > ret_buffer_size + read_position)
					{
						delay_data->copyFrom(channel, 0, copy_data + read_position, ret_buffer_size);
					}
					else
					{
						const int buffer_remaining = delay_samples - read_position;
						delay_data->copyFrom(channel, 0, copy_data + read_position, buffer_remaining);
						delay_data->copyFrom(channel, buffer_remaining, copy_data, ret_buffer_size - buffer_remaining);
					}
				}
			}

			delay_position += ret_buffer_size;

			return delay_data;
		}

		/*std::shared_ptr<juce::AudioBuffer<float>> getDelayData(unsigned int ret_buffer_size)
		{
			if (false && delay_samples > delay_buffer.getNumSamples())
			{
				delay_buffer.setSize(2, delay_samples, true, true, true);
				copy_buffer.setSize(2, delay_samples, true, true, true);
				return std::make_shared<juce::AudioBuffer<float>>(2, ret_buffer_size);
			}

			auto delay_data = std::make_shared<juce::AudioBuffer<float>>(2, ret_buffer_size);

			const int delay_buffer_length = delay_buffer.getNumSamples();
			int read_position = (delay_buffer_length + delay_position - delay_samples) % delay_samples;

 			for (int channel = 0; channel < delay_buffer.getNumChannels(); channel++)
			{
				auto channel_data = delay_data->getWritePointer(channel);
				float* delay_channel_data = delay_buffer.getWritePointer(channel);

				float* copy_data = copy_buffer.getWritePointer(channel);

				if (delay_samples < ret_buffer_size)
				{
					delay_data->copyFrom(channel, 0, delay_channel_data, delay_samples);
					delay_data->copyFrom(channel, delay_samples, copy_buffer, channel, 0, ret_buffer_size - delay_samples);
				}
				else
				{
					if (delay_samples > ret_buffer_size + read_position)
					{
						delay_data->copyFrom(channel, 0, copy_data + read_position, ret_buffer_size);
					}
					else
					{
						const int buffer_remaining = delay_samples - read_position;
						delay_data->copyFrom(channel, 0, copy_data + read_position, buffer_remaining);
						delay_data->copyFrom(channel, buffer_remaining, copy_data, ret_buffer_size - buffer_remaining);
					}
				}
			}

			return delay_data;
		}

		void add_data(juce::AudioBuffer<float>& buffer) 
		{
			auto buffer_length = buffer.getNumSamples();
			auto delay_buffer_length = delay_buffer.getNumSamples();

			for (int channel = 0; channel < delay_buffer.getNumChannels(); channel++)
			{
				auto buffer_channel_data = buffer.getWritePointer(channel);
				auto delay_channel_data = delay_buffer.getWritePointer(channel);

				if (delay_samples < buffer_length)
				{
					copy_buffer.copyFrom(channel, 0, buffer, channel, 0, delay_samples);
					delay_buffer.copyFrom(channel, buffer_length - delay_samples, copy_buffer.getReadPointer(channel), delay_samples);
				}
				else
				{
					if (buffer_length < delay_buffer_length - delay_position)
					{
						delay_buffer.copyFrom(channel, delay_position, buffer_channel_data, buffer_length);
					}
					else
					{
						const int buffer_rem = delay_samples - delay_position;
						delay_buffer.copyFromWithRamp(channel, delay_position, buffer_channel_data, buffer_rem, delay_buffer_ramp_db, delay_buffer_ramp_db);
						delay_buffer.copyFromWithRamp(channel, 0, buffer_channel_data + buffer_rem, buffer_length - buffer_rem, delay_buffer_ramp_db, delay_buffer_ramp_db);
					}
				}
			}

			delay_position += buffer_length;
			delay_position %= delay_samples;
		}*/

		void setDelay(unsigned int delay_samples)
		{
			this->delay_samples = juce::jmax(delay_samples, (unsigned int)1);
			//delay_position %= this->delay_samples;

			if (delay_samples > delay_buffer.getNumSamples())
			{
				delay_buffer.setSize(2, delay_samples, true, true, true);
				copy_buffer.setSize(2, delay_samples, true, true, true);
			}
		}

		unsigned int getDelay()
		{
			return delay_samples;
		}

	private:
		juce::AudioBuffer<float> delay_buffer;
		juce::AudioBuffer<float> copy_buffer;

		unsigned int delay_samples;
		unsigned int delay_position = 0;
	};

}