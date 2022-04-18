/* Copyright (c) 2021, Christian Hertel */

#pragma once

#include <JuceHeader.h>

namespace orpheus {

	class AudioStreamHandler : public juce::ActionBroadcaster
	{
	public:
		virtual ~AudioStreamHandler();

		void init(float sample_rate);
		
		void update(juce::AudioBuffer<float>& buffer, juce::AudioBuffer<float>& host_out, juce::AudioBuffer<float>& guest_out);
		void stop();

		bool isActive();

	protected:
		float sample_rate = 0;
		bool active = false;

		virtual void _stop() = 0;
		virtual void _update(const juce::AudioBuffer<float>&, juce::AudioBuffer<float>& host_out, juce::AudioBuffer<float>& guest_out) = 0;
		virtual bool _init() = 0;

	private:
		juce::CriticalSection critical_section;
	};

#ifdef ORPHEUS_INCLUDE_HOSTSERVER
	class AudioStreamHostServerHandler : public AudioStreamHandler
	{
	public:
		bool _init() override
		{
			if (server.get() == nullptr)
			{
				server = std::make_unique<::orpheus::AudioStreamServer>(this->sample_rate);
				return server->beginWaitingForSocket(ORPHEUS_PORT_GUEST);
			}

			return false;
		}

		void _update(const juce::AudioBuffer<float>& buffer, juce::AudioBuffer<float>& host_out, juce::AudioBuffer<float>& guest_out)
		{
			const auto buffer_size = buffer.getNumSamples();
			auto guest_connection = server->getConnection(0);

			//default: no host out, no guest out
			host_out.clear();
			guest_out.clear();

			if (guest_connection != nullptr && guest_connection->isConnected() == false)
			{
				stop();
				return;
			}

			if (guest_connection != nullptr) {
				//data to server
				server->add_data(buffer);
				//cache data
				host_cache.add_data(buffer);

				std::shared_ptr<juce::AudioBuffer<float>> guest_data = nullptr;
				guest_data = (guest_connection->getRecievedData().getSize() >= ORPHEUS_RECDATA_BUFFERS) ? guest_connection->getNextSamples(buffer_size) : nullptr;
				if (guest_data.get() != nullptr) {
					host_out = *host_cache.getNextChunk(buffer_size);

					guest_out = *guest_data.get();
				}

			}
		}

		void _stop() override
		{
			server->stop();
			server.reset();
		}

	private:
		orpheus::PipeBuffer host_cache;
		std::unique_ptr<AudioStreamServer> server;
	};
#endif

}