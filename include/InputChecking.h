#pragma once
#include <iostream>
#include <string>
#include <algorithm>
#include "external/raknet/Source/MessageIdentifiers.h"
#include "external/raknet/Source/RakPeerInterface.h"
enum class START_INPUT
{
    HOST,
    JOIN
};
class InputChecking
{
public:
	static START_INPUT GetHostJoinInput()
	{
		bool success = false;
		while (!success)
		{
            std::cout << "[H] / [J]" << std::endl;
            std::string input;
            std::getline(std::cin, input);

            std::transform(input.begin(), input.end(), input.begin(), toupper);

            // Error checking for input.
            if (input.compare("H") == 0)
            {
                success = true; // They entered correct input.
                
                // They want to host.
                return START_INPUT::HOST;
            }
            else if (input.compare("J") == 0)
            {
                success = true; // They entered correct input.
                // They want to join a server.
                return START_INPUT::JOIN;
            }
            else
            {
                success = false; // Incorrect input.
                // They typed the wrong input, we should get them to repeat that.
                continue;
            }
		}
	}

    static void GetHostAddressInput(std::string& ipAddress, unsigned short& portNum)
    {
        bool success = false;
        std::string ip;
        unsigned short port;

        std::string portString;

        std::cout << "Please enter IP address of the server." << std::endl;
        std::getline(std::cin, ip);
        
        std::cout << "IP address entered: " << ip << std::endl;

        while (!success)
        {
            std::cout << "Please enter port number of the server." << std::endl;
            std::getline(std::cin, portString);

            try
            {
                port = (unsigned short)std::strtoul(portString.c_str(), NULL, 0);
                success = true;
            }
            catch (...)
            {
                std::cout << "Entered port number could not be converted into an unsigned short! Please try again." << std::endl;
                success = false;
                continue;
            }
        }

        // Apply changes to reference parameters.
        ipAddress = ip;
        portNum = port;
        

    }

    static unsigned char GetPacketIdentifier(RakNet::Packet* p)
    {
        if ((unsigned char)p->data[0] == ID_TIMESTAMP)
            return (unsigned char)p->data[sizeof(RakNet::MessageID) + sizeof(RakNet::Time)];
        else
            return (unsigned char)p->data[0];
    }

};