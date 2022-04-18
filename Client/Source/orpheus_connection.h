/* Copyright (c) 2021, Christian Hertel */

/**
* This file contains the definition of the abstract orpheus::AudioStreamConnection class.
* It represents a network connection socket and handles basic functionality, i.e. sending
* audio data (async), follow the Oprheus network protocol, buffer recieved data etc.
*/

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
		/**
		* Tries to connect the socket to a listening Orpheus server.
		* @return : Connection succeded
		* @param address : IP-address of the server
		* @param port : server port
		*/
		bool connect(const juce::String& address, unsigned int port);

		/**
		* Adds audio data to the internal buffer. Subclasses implement specific storage
		* and flushing/sending behaviour of the class.
		* @param buffer : Audio data to store
		*/
		virtual void add_data(const juce::AudioBuffer<float>& buffer) = 0;

		/**
		* Returns the next audio data chunk which was recieved via the network connection
		* @return : Pointer to the audio data chunk
		* @param numSamples : expected size of the returned audio buffer
		*/
		std::shared_ptr<juce::AudioBuffer<float>> getNextSamples(int numSamples);

		/**
		* Set the sample rate of the audio stream.
		* @param sample_rate : new sample rate
		*/
		void setSampleRate(float sample_rate);

		/**
		* Get the sample rate of the network audio stream.
		*/
		float getSampleRate();

		/**
		*	Returns the audio data recieved via the network audio stream.
		*/
		const PipeBuffer& getRecievedData();

		/**
		* Returns the audio data added to the pending audio data packets queue.
		* NOTE: Audio packets wich were send already are removed from this buffer.
		*/
		const PipeBuffer& getAddedData();

		/**
		* Returns true if the audio connection was successfully initialised and
		* an other client has entered the session.
		*/
		bool isReadyToStream()
		{
			return readyToStream;
		}

		/**
		* Shuts down the audio connection.
		*/
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
