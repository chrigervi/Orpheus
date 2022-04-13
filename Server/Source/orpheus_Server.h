/** (c) Christian Hertel 2022 */

/**
* This file contains the definition and the implementation
* of orpheus::ServerConnection and orpheus::Server.
* In order to start an Orpheus session both Host-CLient and Guest-Client
* need to have a valid connection to the same Orpheus Server instance.
*/

#pragma once
#include "JuceHeader.h"
#include <vector>


/** Constants used within the Oprheus network protocol (look at Client/orpheus_config.h)*/
/// Package starter for approving incoming client connections (Server --> CLient)
#define ORPHEUS_CONNECTION_APPROVED 6868927612724466250
/// Package starter for refusing incoming client connections (Server --> Client)
#define ORPHEUS_CONNECTION_REFUSED 6868927612724466251
/// Package starter to determine the client's role as a GUEST within a Oprheus session (Client --> Server)
#define ORPHEUS_CONNECTION_TYPE_GUEST 6868927612724466252
/// Package starter to determine the client's role as a HOST within a Oprheus session (Client --> Server)
#define ORPHEUS_CONNECTION_TYPE_HOST 6868927612724466253
/// Package starter to initiate the audio stream (Server --> Client)
#define ORPHEUS_START_STREAM 6868927612724466254
/// Package starter to stop the audio stream (Server --> Client)
#define ORPHEUS_STOP_STREAM 6868927612724466255
/// Package starter to mark a packet containing streamed audio data (Client --> Server --> CLient)
#define ORPHEUS_STREAM_PACKET 6868927612724466256


namespace orpheus
{
	class ServerConnection : public juce::InterprocessConnection
	{
	private:
		bool _connectedWithHost = false;
		bool _legitConnection = false;
		ServerConnection* _otherPeer = nullptr;

		void sendMessageCode(juce::int64 code)
		{
			juce::MemoryBlock mb;
			mb.append(&code, sizeof(juce::int64)); // Size should be 8 bytes (long long)

			sendMessage(mb);
		}

		void connectionMade() override
		{
			juce::Thread::launch([this] {
				std::cout << "New Connection with " << getConnectedHostName() << std::endl;
				doOrpheusHandshake();
				if (_legitConnection) std::cout << "Connection confirmed!" << std::endl;
			});
		}
		void connectionLost() {}

	public:
		bool isConnectedWithHost() { return _connectedWithHost; }

		void doOrpheusHandshake()
		{
			unsigned int numTries = 3;
			while (numTries-- > 0 && !_legitConnection)
			{
				juce::Thread::getCurrentThread()->sleep(1000);
			}

			if (_legitConnection)
			{

			}
			else
			{
				refuseAndClose();
			}
		}

		void messageReceived(const juce::MemoryBlock& message)
		{
			// Messages smaller than 8 bytes are ignored
			if (message.getSize() < 8) return;

			juce::int64 msgcode = *(juce::int64*)message.getData();

			// Is the connection already approved and initialised?
			if (!_legitConnection)
			{
				switch (msgcode)
				{
				case ORPHEUS_CONNECTION_TYPE_GUEST:
					_connectedWithHost = false;
					_legitConnection = true;
					break;
				case ORPHEUS_CONNECTION_TYPE_HOST:
					_connectedWithHost = true;
					_legitConnection = true;
					break;
				default:
					_legitConnection = false;
					refuseAndClose();
					break;
				}

				return;
			}

			switch (msgcode)
			{
			case ORPHEUS_STOP_STREAM:
				if (_otherPeer != nullptr)
				{
					_otherPeer->sendMessage(message);
					std::cout << getConnectedHostName() << " : STOP_STREAM" << std::endl;
				}
				break;
			case ORPHEUS_STREAM_PACKET:
				if (_otherPeer != nullptr)
				{
					_otherPeer->sendMessage(message);
					std::cout << getConnectedHostName() << " -> " << _otherPeer->getConnectedHostName() << " : " << message.getSize() << "bytes" << std::endl;
				}
				break;
			}
		}

		void approve()
		{
			std::cout << "Connection to " << getConnectedHostName() << " was approved!" << std::endl;

			sendMessageCode(ORPHEUS_CONNECTION_APPROVED);
		}

		void startStreaming(ServerConnection* otherPeer)
		{
			_otherPeer = otherPeer;
			sendMessageCode(ORPHEUS_START_STREAM);
		}

		void stopStreamAndClose()
		{
			sendMessageCode(ORPHEUS_STOP_STREAM);
			disconnect();
		}

		void refuseAndClose()
		{
			std::cout << "Connection to " << getConnectedHostName() << " was refused!" << std::endl;

			sendMessageCode(ORPHEUS_CONNECTION_REFUSED);
			disconnect();
		}
	};

	class Server : public juce::InterprocessConnectionServer, private juce::Timer
	{
	private:
		struct Session
		{
			ServerConnection* peer1 = nullptr;
			ServerConnection* peer2 = nullptr;

			bool isInitialised()
			{
				return peer1 != nullptr && peer2 != nullptr;
			}

			bool isValid()
			{
				return (peer1 != nullptr && peer2 != nullptr)
					&& (peer1->isConnectedWithHost() != peer2->isConnectedWithHost())
					&& (peer1->isConnected() && peer2->isConnected());
			}
		};

		void deleteClosedConnections()
		{
			for (int i = 0; i < _connections.size(); i++)
			{
				if (_connections[i]->isConnected() == false)
				{
					delete _connections[i];
					_connections.erase(_connections.begin() + i);
					i--;
				}
			}
		}

		void timerCallback() override
		{
			deleteClosedConnections();

			if (_connections.size() > 0 && session.peer1 == nullptr)
			{

				session.peer1 = _connections[0];
				session.peer1->approve();

				return;
			}

			if (session.isInitialised() && session.isValid() == false)
			{
				std::cout << "Delete Session" << std::endl;

				session.peer1->stopStreamAndClose();
				session.peer2->stopStreamAndClose();

				delete session.peer1;
				session.peer1 = nullptr;

				delete session.peer2;
				session.peer2 = nullptr;
			}

			if (_connections.size() > 1)
			{
				if (session.peer1->isConnectedWithHost() != _connections[1]->isConnectedWithHost())
				{
					session.peer2 = _connections[1];
					session.peer2->approve();

					session.peer1->startStreaming(session.peer2);
					session.peer2->startStreaming(session.peer1);

					for (int i = 0; i < 2; i++)
						_connections.erase(_connections.begin());
				}
				else
				{
					_connections[1]->refuseAndClose();
				}
			}
		}

		std::vector<ServerConnection*> _connections;
		Session session;

	public:
		Server(unsigned int port)
		{
			startTimer(500);

			beginWaitingForSocket(port);

			std::cout << "Server is running on port " << port << std::endl;
		}

		~Server()
		{
			for (int i = 0; i < _connections.size(); i++)
			{
				delete _connections[i];
			}

			delete session.peer1;
			delete session.peer2;
		}

		juce::InterprocessConnection* createConnectionObject() override
		{
			auto conn = new ServerConnection();
			_connections.push_back(conn);
			return conn;
		}
	};
}
