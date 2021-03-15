#include "orpheus_net.h"

#ifdef ORPHEUS_INCLUDE_HOSTSERVER
//========= Audio Stream Server =========
orpheus::AudioStreamServer::AudioStreamServer(float sample_rate, size_t max_audio_buffer_size)
	: max_buffer_size(max_audio_buffer_size), audio_buffer_send(2, max_audio_buffer_size), sample_rate(sample_rate)
{}
orpheus::AudioStreamServer::~AudioStreamServer()
{
	ORPHLOG("Deleting all connections...");

	for (int i = 0; i < connections.size(); i++)
	{
		delete connections[i];
	}
	connections.clear();
}
void orpheus::AudioStreamServer::add_data(const juce::AudioBuffer<float>& buffer)
{
	for (int i = 0; i < connections.size(); i++)
	{
		connections[i]->add_data(buffer);
	}
}
orpheus::AudioStreamHostConnection* orpheus::AudioStreamServer::getConnection(unsigned int index)
{
	if (index >= connections.size())
		return nullptr;

	return connections.at(index);
}
juce::InterprocessConnection* orpheus::AudioStreamServer::createConnectionObject() {
	orpheus::println("new connection established!", "Server");

	//juce::InterprocessConnection* connection = new AudioStreamDummyConnection();
	auto connection = new AudioStreamServerConnection(sample_rate, ORPHEUS_AUDIOSTREAM_BSIZE);
	connections.push_back(connection);

	return connection;
}
#endif