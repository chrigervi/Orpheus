/* Copyright (c) 2021, Christian Hertel */

#include "orpheus_connection.h"
#include "sandbox.h"
#include "orpheus_DataTracker.h"

//=========Audio Stream Connection=========
orpheus::AudioStreamConnection::AudioStreamConnection(juce::int64 netID, float sample_rate, size_t chunkSize)
	: chunkSize(chunkSize), _asyncSender(this), _netID(netID)
{
	setSampleRate(sample_rate); // Track packets/s
}

orpheus::AudioStreamConnection::~AudioStreamConnection()
{
	_asyncSender.stopThread(2000);
    closeConnection();
}

void orpheus::AudioStreamConnection::streamCache()
{
	if ((cache_data_added.getSize() / chunkSize) <= chunksToStream)
	{
		ORPHLOG("Cache does not contain enough data to stream ...");
		return;
	}

	chunksToStream += 1;
}

void orpheus::AudioStreamConnection::send_data(const void* data, size_t dataSize)
{
	if (data == nullptr || dataSize == 0) return;

	//auto dataCompressed = stream_data::compressData(data, dataSize);
	stream_data strdata(ORPHEUS_STREAM_PACKET, this->sample_rate, data, dataSize);

	send_data(strdata);
}
void orpheus::AudioStreamConnection::send_data(stream_data& strdata)
{
	byte* strdata_pntr = nullptr;
	size_t strdata_size = strdata.convertToDataPointer(strdata_pntr);

	juce::MemoryBlock mb(strdata_pntr, strdata_size);
	sendMessage(mb);

	this->_totalDataSend_mb += float(strdata_size) / (1024.0f * 1024.0f);
	ORPH_TRACKDATA("Total streamed (MB)", _totalDataSend_mb);
}

void orpheus::AudioStreamConnection::sendMessageCode(juce::int64 code)
{
	juce::MemoryBlock mb;
	mb.append(&code, sizeof(juce::int64)); // Should be 8 bytes

	sendMessage(mb);

	ORPHLOG("Messagecode send: " + std::to_string(code));
}

bool orpheus::AudioStreamConnection::connect(const juce::String& address, unsigned int port)
{
	bool success = connectToSocket(address, port, 2000);

	if (!success) return false;

	unsigned int numTries = 6;
	while (numTries-- > 0 && _approvedByServer == -1) // -1 = undefined
	{
		juce::Thread::getCurrentThread()->sleep(1000);
	}

	return _approvedByServer == 1;
}

std::shared_ptr<juce::AudioBuffer<float>> orpheus::AudioStreamConnection::getNextSamples(int numSamples)
{	
	if (_minimumBufferSizeReached == false) 
		return nullptr;

	if (numSamples > cache_data_recieved.getSize())
	{
		ORPHLOG("Audio stream is to slow");
		return nullptr;
	}

	return cache_data_recieved.getNextChunk(numSamples);
}

void orpheus::AudioStreamConnection::setSampleRate(float sample_rate)
{
	this->sample_rate = sample_rate;
	ORPH_TRACKDATA("Packets/s", (1.0f / sample_rate) * ORPHEUS_AUDIOSTREAM_BSIZE);
}

float orpheus::AudioStreamConnection::getSampleRate()
{
	return sample_rate;
}

const orpheus::PipeBuffer& orpheus::AudioStreamConnection::getRecievedData()
{
	return this->cache_data_recieved;
}

const orpheus::PipeBuffer& orpheus::AudioStreamConnection::getAddedData()
{
	return this->cache_data_added;
}

void orpheus::AudioStreamConnection::closeConnection()
{
	juce::ScopedLock lock(critical_section);

	//send STOPSTREAM-code to the other peer
	juce::int64 msgcode = ORPHEUS_STOP_STREAM;
	juce::MemoryBlock mb(&msgcode, sizeof(juce::int64));
	
	//the connection could already be closed
	try 
	{
		if (isReadyToStream()) sendMessage(mb);
		this->disconnect();
	}
	catch (std::exception e) {}
}

void orpheus::AudioStreamConnection::connectionMade() {
	orpheus::println("connected!", "AudioStreamConnection");

	// Orpheus handshake
	sendMessageCode(_netID);
}

void orpheus::AudioStreamConnection::connectionLost() {
	orpheus::println("connection lost!", "AudioStreamConnection");
}

void orpheus::AudioStreamConnection::messageReceived(const juce::MemoryBlock& message)
{
	//ORPHLOG("recieved " + std::to_string(message.getSize()) + " bytes");
	juce::ScopedLock lock(critical_section);

	if (message.getSize() < 8)
	{
		ORPHLOG("Message smaller than 8 bytes is ignored");
		return;
	}

	juce::int64 messageHeader = *(juce::int64*)message.getData();

	switch (messageHeader)
	{
	case ORPHEUS_CONNECTION_APPROVED:
		_approvedByServer = 1;
		ORPHLOG("Connection approved");
		break;
	case ORPHEUS_CONNECTION_REFUSED:
		_approvedByServer = 0;
		ORPHLOG("Connection refused");
		closeConnection();
		break;
	case ORPHEUS_START_STREAM:
		readyToStream = true;
		_asyncSender.startThread();
		ORPHLOG("Ready to stream!");
		break;
	case ORPHEUS_STOP_STREAM:
		readyToStream = false;
		_asyncSender.stopThread(2000);
		closeConnection();
		break;
	case ORPHEUS_STREAM_PACKET:
		//Fetch the packet
		const byte* strdata_pntr = (const byte*)message.getData();
		size_t strdata_size = message.getSize();
		stream_data strdata(strdata_pntr, strdata_size);

		//unequal sample rates are not supported
		if (strdata.sample_rate != this->sample_rate)
		{
			//TODO: Error
		}

		//Decompress audio data
		//TODO: This copies the data needlessly often --> overhead
		auto decompr = stream_data::decompressAudioData(strdata.data, strdata.data_size, ORPHEUS_AUDIOSTREAM_BSIZE);
		float* decompr_data;
		size_t decompr_dataSize = orpheus::PipeBuffer::convertToFloatSequence(*decompr, decompr_data);
		stream_data strdataDecompr(strdata.identification, strdata.sample_rate, decompr_data, decompr_dataSize * sizeof(float));
		delete[] decompr_data;

		// Add audio data to the output buffer.
		// BUFFERING: If the buffer contains enough data _minimBufferSizeReached is set to true --> getNextSamples() will return data.
		cache_data_recieved.add_data((float*) strdataDecompr.data, strdataDecompr.data_size / sizeof(float));
		if (cache_data_recieved.getSize() >= ORPHEUS_RECDATA_BUFFERS)
		{
			_minimumBufferSizeReached = true;
		}

		onStreamPacketRecieved(strdataDecompr);

		_totalDataRecieved_mb += float(strdata_size) / (1024.0f * 1024.0f);
		ORPH_TRACKDATA("Total recieved (MB)", _totalDataRecieved_mb);
		ORPH_TRACKDATA("Packet-Size (KB)", float(strdata_size) / 1024.0f);

		break;
	}
}

orpheus::AudioStreamConnection::_AsyncSender::_AsyncSender(AudioStreamConnection* conn)
	: Thread("AudioStreamConnection::_AsyncSender"), connection(conn)
{}

orpheus::AudioStreamConnection::_AsyncSender::~_AsyncSender() {}

void orpheus::AudioStreamConnection::_AsyncSender::run()
{
	while (!threadShouldExit())
	{
		if (connection != nullptr && connection->chunksToStream > 0 && connection->cache_data_added.getSize() >= connection->chunkSize)
		{
			std::shared_ptr<juce::AudioBuffer<float>> buffer = nullptr;
			//thread safe
			{
				juce::ScopedLock lock(connection->critical_section);
                
				buffer = connection->cache_data_added.getNextChunk(connection->chunkSize);
                connection->chunksToStream -= 1;
			}

			auto compressed = stream_data::compressAudioData(*buffer, connection->sample_rate);
			connection->send_data(compressed->getData(), compressed->getSize());
		}

		sleep(20);
	}
}
