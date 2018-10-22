#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#include <cstdio>
#include <vector>
#include "PacketReader.h"
#include "FormattedPacket.h"
#pragma comment(lib, "Ws2_32.lib")
#include <winsock2.h>

/// <summary>
/// Main method for sending packets captured into a file over UDP
/// </summary>
/// <returns>0 upon successful completion</returns>
int main2()
{
	//Preliminary setup for winsocket
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);	//Decision to use MAKEWORD obtained from comment at https://docs.microsoft.com/en-us/windows/desktop/winsock/initializing-winsock

	//The socket we'll bind to
	int socketUDP = socket(AF_INET, SOCK_DGRAM, 0);

	//Read the PCAP packet captures into my FormattedPacket (and data) data structures
	std::vector<FormattedPacket> packets = read_packet(
		R"(C:\Users\DEMcKnight\source\repos\WinPcapSender\Files\Project1GradedInput.pcap)");

	//For each packet, send its contents over the UDP connection
	for (FormattedPacket packet : packets)
	{
		//sockaddr_in information taken from https://beej.us/guide/bgnet/html/multi/sockaddr_inman.html
		struct sockaddr_in serverInfo;
		serverInfo.sin_family = AF_INET;
		serverInfo.sin_port = htons(443);
		serverInfo.sin_addr.s_addr = inet_addr("127.3.1.4");

		std::cout << "Sending:..." << std::endl;
		std::cout << packet << std::endl;

		while (true)
		{
			//Prepare an array to retrieve the response
			char response[1024];
			std::fill(response, response + sizeof(response), 0);

			//Decide upon the choice of transmission and the transmission size
			const int transmissionSize = packet.hexData.size();
			char* transmissionData = new char[transmissionSize + 1];
			strcpy(transmissionData, packet.hexData.c_str());

			int serverInfoSize = sizeof(serverInfo);	//The size of the sockaddr_in struct
			//If we successfully send the packet
			if (sendto(socketUDP, transmissionData, transmissionSize, 0, (sockaddr*)&serverInfo, serverInfoSize) != SOCKET_ERROR)
			{
				//And successfully retrieve the packet
				if (recvfrom(socketUDP, response, sizeof(response), 0, (sockaddr*)&serverInfo, &serverInfoSize) != SOCKET_ERROR)
				{
					//Then break, allowing for the sending of the next packet.
					printf("Response received: %s\n", response);
					//Sleep(1000);
					break;
				}
			}
		}
	}

	//Close the socket now that we're done with it.
	closesocket(socketUDP);
	std::cout << "Press 'x', then 'enter'" << std::endl;
	std::string s;
	std::cin >> s;

	return 0;
}