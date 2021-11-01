#include "Player.h"
#include "NetworkSettings.h"

Player::Player(std::string username)
{
	m_username = username;
}

/// <summary>
/// Returns true if successful and false if failed.
/// </summary>
/// <param name="input"></param>
/// <returns></returns>
bool Player::InitNetworking(START_INPUT input)
{
	switch (input)
	{
		case START_INPUT::HOST:
		{
			peer = RakNet::RakPeerInterface::GetInstance();
			RakNet::StartupResult result;
			result = peer->Startup(MAX_CONNECTIONS, &RakNet::SocketDescriptor(SERVER_PORT, 0), 1);
			if (result == RakNet::StartupResult::RAKNET_STARTED)
			{
				std::cout << "Server started successfully!" << std::endl;
				std::cout << "The server is running on port: " << SERVER_PORT << std::endl;
				peer->SetMaximumIncomingConnections(MAX_INCOMING_CONNECTIONS);
				return true;
			}
			else
			{
				std::cerr << "Server could not start." << std::endl;
				return false;
			}
			break;
		}
		case START_INPUT::JOIN:
		{
			peer = RakNet::RakPeerInterface::GetInstance();
			RakNet::StartupResult result;
			result = peer->Startup(1, &RakNet::SocketDescriptor(), 1);


			// Now try to connect to server.
			std::string ip;
			unsigned short port;
			InputChecking::GetHostAddressInput(ip, port);

			peer->Connect(ip.c_str(), port, 0, 0);

			std::cout << "Client started successfully!" << std::endl;
			return true;
		}
	}
}
