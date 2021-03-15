/* Copyright (c) 2021, Christian Hertel */

#include "orpheus_stream_data.h"

#include "orpheus_DataTracker.h"
#include "sandbox.h"

//INIT STATIC:
unsigned int orpheus::stream_data::_audioCompressionQualiyIndex = ORPHEUS_AUDIOCOMPRESSION_STD_QUALITY_INDEX;

//========stream_data========
orpheus::stream_data::stream_data(juce::uint64 identification, const float sample_rate, const void* data, size_t data_size)
	: identification(identification), sample_rate(sample_rate), data_size(data_size)
{
	this->data = new byte[data_size];
	memcpy(this->data, data, data_size);
}
orpheus::stream_data::stream_data(const byte* stream_data_pointer, size_t stream_data_pointer_size) :
#define dstart ((float*)(stream_data_pointer + sizeof(juce::uint64) + sizeof(float) + sizeof(size_t)))
	identification(*(juce::uint64*)stream_data_pointer),
	sample_rate(*(float*)(stream_data_pointer + sizeof(juce::uint64))),
	data_size(*(size_t*)(stream_data_pointer + sizeof(juce::uint64) + sizeof(float)))
{
	data = new byte[data_size];
	memcpy(data, dstart, data_size);
#undef dstart
#undef dlength
}
orpheus::stream_data::~stream_data()
{
	delete[] data;
	data = nullptr;

	delete[] stream_data_pointer;
	stream_data_pointer = nullptr;
}
size_t orpheus::stream_data::convertToDataPointer(byte*& ret_pointer)
{
	if (stream_data_pointer != nullptr)
	{
		ret_pointer = stream_data_pointer;
		return stream_data_size;
	}

	stream_data_size = sizeof(identification) + sizeof(sample_rate) + data_size + sizeof(data_size);

	// make the data length a multiple of 2048 bytes:
	// JUCE ADDS ADDITIONAL 8 BYTES TO THE HEADER OF THE PACKET!
	const unsigned int fit = 8;
	int restToFit = (stream_data_size + 8) % fit;
	if (restToFit != 0)
	{
		stream_data_size += fit - restToFit;
	}

	stream_data_pointer = new byte[stream_data_size];
	unsigned int write_pos = 0;
	// signature code
	memcpy(stream_data_pointer + write_pos, &identification, sizeof(juce::uint64));
	write_pos += sizeof(juce::uint64);
	// Sample rate
	memcpy(stream_data_pointer + write_pos, &sample_rate, sizeof(float));
	write_pos += sizeof(float);
	// Size of audio data
	memcpy(stream_data_pointer + write_pos, &data_size, sizeof(data_size));
	write_pos += sizeof(data_size);
	// audio data
	memcpy(stream_data_pointer + write_pos, data, data_size);
	write_pos += data_size;


	ret_pointer = stream_data_pointer;
	return stream_data_size;
}

std::shared_ptr<juce::MemoryOutputStream> orpheus::stream_data::compressData(float* data, size_t data_size)
{
	auto ostream = std::make_shared<juce::MemoryOutputStream>();
	juce::GZIPCompressorOutputStream comprstream((juce::OutputStream*)ostream.get(), ORPHEUS_COMPRESSION_LEVEL, false, 0);
	jassert(comprstream.write(data, data_size * sizeof(float)));
	comprstream.flush();

	return ostream;
}
size_t orpheus::stream_data::decompressData(void* dataIn, size_t dataIn_size, float*& dataOut)
{
	auto istream = juce::MemoryInputStream(dataIn, dataIn_size, false);
	juce::GZIPDecompressorInputStream decomprstream(&istream, false);

	size_t dataOut_size = ORPHEUS_AUDIOSTREAM_BSIZE * 2;
	dataOut = new float[dataOut_size];
	size_t readBytes = decomprstream.read(dataOut, dataOut_size * sizeof(float));

	jassert(readBytes == dataOut_size * sizeof(float));

	return dataOut_size;
}

void orpheus::stream_data::setAudioCompressionQualityIndex(unsigned int qualityIndex)
{
    _audioCompressionQualiyIndex = qualityIndex;
}

std::shared_ptr<juce::MemoryBlock> orpheus::stream_data::compressAudioData(const juce::AudioSampleBuffer& audioData, float sampleRate)
{
	auto compressedData = std::make_shared<juce::MemoryBlock>();
	juce::MemoryOutputStream* ostream = new juce::MemoryOutputStream(*compressedData.get(), false);

    ORPHEUS_AUDIOCOMPRESSION_JUCE_CODEC codec;
	juce::StringPairArray metadata;
	metadata.set("encoderName", "Orpheus");

	auto comprWriter = codec.createWriterFor(ostream, sampleRate, audioData.getNumChannels(), ORPHEUS_AUDIOCOMPRESSION_BITS_PER_SAMPLE, metadata, ORPHEUS_AUDIOCOMPRESSION_STD_QUALITY_INDEX);
	bool success = comprWriter->writeFromAudioSampleBuffer(audioData, 0, audioData.getNumSamples());
	delete comprWriter;

	//log Compression-rate
	size_t sizeUncomp = audioData.getNumSamples() * audioData.getNumChannels() * sizeof(float);
	size_t sizeComp = compressedData->getSize();
	float rate = (float)sizeComp / (float)sizeUncomp;
	ORPH_TRACKDATA("Compression-Rate", rate);

	return compressedData;
}

std::shared_ptr<juce::AudioSampleBuffer> orpheus::stream_data::decompressAudioData(const void* dataIn, size_t dataIn_size, unsigned int numSamples)
{
	auto istream = new juce::MemoryInputStream(dataIn, dataIn_size, true);
    ORPHEUS_AUDIOCOMPRESSION_JUCE_CODEC codec;

	std::shared_ptr<juce::AudioSampleBuffer> buffer = std::make_shared<juce::AudioSampleBuffer>(2, numSamples);

	auto comprReader = codec.createReaderFor(istream, true);
	if (comprReader != nullptr)
	{
		comprReader->read(buffer.get(), 0, numSamples, 0, true, true);

		delete comprReader;
	}
	else
	{
		orpheus::println("Could not decompress audio data");
	}

	return buffer;
}
