/* Copyright (c) 2021, Christian Hertel */

#pragma once

#include "orpheus_config.h"
#include "orpheus_pipe_buffer.h"
#include "orpheus_stream_data.h"
#include "orpheus_log.h"

namespace orpheus
{
	class AudioStreamConnection : public juce::InterprocessConnection
	{
	public:
		bool connect(const juce::String& address, unsigned int port);

		virtual void add_data(const juce::AudioBuffer<float>& buffer) = 0;

		std::shared_ptr<juce::AudioBuffer<float>> getNextSamples(int numSamples);

		void setSampleRate(float sample_rate);
		float getSampleRate();
		
		const PipeBuffer& getRecievedData();
		const PipeBuffer& getAddedData();

		bool isReadyToStream()
		{
			return readyToStream;
		}

		void closeConnection();

	protected:
		AudioStreamConnection(juce::int64 netID, float sample_rate, size_t chunkSize = ORPHEUS_AUDIOSTREAM_BSIZE);
		virtual ~AudioStreamConnection();

		void streamCache();

		virtual void onStreamPacketRecieved(stream_data& packet) = 0;

		juce::CriticalSection critical_section;

		::orpheus::PipeBuffer cache_data_added;
		::orpheus::PipeBuffer cache_data_recieved;

		size_t chunkSize;
		unsigned int chunksToStream = 0;

	private:
		juce::int64 _netID = 0;
		int _approvedByServer = -1;

		float sample_rate = 0;
		bool readyToStream = false;
		bool _minimumBufferSizeReached = false;

		float _totalDataSend_mb = 0;
		float _totalDataRecieved_mb = 0;

		void send_data(const void* data, size_t dataSize);
		void send_data(stream_data& strdata);
		void sendMessageCode(juce::int64 code);

		class _AsyncSender : public juce::Thread 
		{
		public:
			_AsyncSender(AudioStreamConnection* conn);

			virtual ~_AsyncSender();

			void run() override;

		private:
			AudioStreamConnection* connection = nullptr;
		} _asyncSender;

		void connectionMade() override;
		void connectionLost() override;
		
		void messageReceived(const juce::MemoryBlock& message) override;
	};
}
