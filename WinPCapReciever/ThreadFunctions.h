#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <string>
#include <vector>
#include <winsock2.h>
#include "FormattedPacket.h"
#include "PacketReader.h"
#include <mutex>

std::mutex control;

/**
 * \brief Function to receive packets set to the provided IP Address
 * \param local_ip The IP Address for which this function should receive packets
 */
inline void receive_packets(int port_number)
{
	std::string local_ip = "";
	/*Setup work for retrieving transmission*/
	//Preliminary setup for winsocket
	//WSADATA wsa_data;
	//WSAStartup(MAKEWORD(2, 2), &wsa_data);	////Decision to use MAKEWORD obtained from comment at https://docs.microsoft.com/en-us/windows/desktop/winsock/initializing-winsock

	//Prepare the socket we're binding to
	int socket_udp = socket(AF_INET, SOCK_DGRAM, 0);

	//Read the retrieved packet captures into my FormattedPacket (and data) data structures
	auto packets = std::vector<FormattedPacket>();

	//sockaddr_in information taken from https://beej.us/guide/bgnet/html/multi/sockaddr_inman.html
	struct sockaddr_in local_info {};
	struct sockaddr_in server_info {};
	local_info.sin_family = AF_INET;
	local_info.sin_port = htons(port_number);
	local_info.sin_addr.s_addr = INADDR_ANY;
	control.lock();
	{
		bind(socket_udp, (sockaddr*)&local_info, sizeof(local_info));
	}
	control.unlock();
	printf("Awaiting Transmission over port %i...\n", port_number);
	while (true)
	{
		control.lock();
		{
			//Create and clear an array to hold the response
			char response[1024];
			std::fill(response, response + sizeof(response), 0);

			//printf("Awaiting Transmission over port %i...\n", port_number);

			/*Retrieve responses*/
			int serverInfoLength = sizeof(server_info);	//The size of the serverInfo sockaddr_in struct
			//If we receive a transmission
			if (recvfrom(socket_udp, response, sizeof(response), 0, (sockaddr*)&server_info, &serverInfoLength) != SOCKET_ERROR)
			{
				//Print the transmission and where we got it, then proceed to send an affirming response
				printf("Transmission from address %s received: %s\n", inet_ntoa(server_info.sin_addr), response);
				packets.emplace_back(FormattedPacket(response));
				std::cout << packets[packets.size() - 1] << std::endl;
				sendto(socket_udp, response, sizeof(response), 0, (sockaddr*)&server_info, serverInfoLength);
			}
		}
		control.unlock();
	}
	closesocket(socket_udp);
}

inline bool send_packet(sockaddr_in reception_socket, FormattedPacket packet)
{
	const int socket_udp = socket(AF_INET, SOCK_DGRAM, 0);

	//Prepare an array to retrieve the response
	char response[1024];
	std::fill(response, response + sizeof(response), 0);

	//Decide upon the choice of transmission and the transmission size
	const int transmissionSize = packet.hexData.size();
	char* transmissionData = new char[transmissionSize + 1];
	strcpy(transmissionData, packet.hexData.c_str());

	int serverInfoSize = sizeof(reception_socket);	//The size of the sockaddr_in struct
	//If we successfully send the packet
	if (sendto(socket_udp, transmissionData, transmissionSize, 0, (sockaddr*)&reception_socket, serverInfoSize) != SOCKET_ERROR)
	{
		//And successfully retrieve the packet
		if (recvfrom(socket_udp, response, sizeof(response), 0, (sockaddr*)&reception_socket, &serverInfoSize) != SOCKET_ERROR)
		{
			//Then break, allowing for the sending of the next packet.
			printf("Response received: %s\n", response);
			return true;
		}
	}
	return false;
}

inline void send_packets(const std::string local_ip, const int port_number,  std::vector<std::string> neighbor_ips, std::vector<int> neighbor_ports)
{
	//Preliminary setup for winsocket
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);	//Decision to use MAKEWORD obtained from comment at https://docs.microsoft.com/en-us/windows/desktop/winsock/initializing-winsock

	//The sockets we'll bind to
	


	//Read the PCAP packet captures into my FormattedPacket (and data) data structures
	std::vector<FormattedPacket> packets = read_packet(
		R"(C:\Users\DEMcKnight\source\repos\WinPcapSender\Files\Project1GradedInput.pcap)");

	//For each packet, send its contents over the UDP connection
	for (FormattedPacket packet : packets)
	{
		//If the packet's source matches our own
		//if (packet.ipHeader.source == local_ip)
		{
			//sockaddr_in information taken from https://beej.us/guide/bgnet/html/multi/sockaddr_inman.html
			struct sockaddr_in server_info{};
			server_info.sin_family = AF_INET;
			server_info.sin_port = htons(443);
			server_info.sin_addr.s_addr = inet_addr("127.3.1.4");

			control.lock();
			{
				std::cout << "Sending:..." << std::endl;
				std::cout << packet << std::endl;
			}
			control.unlock();

			while (true)
			{
				bool success = false;
				control.lock();
				{
					if(send_packet(server_info, packet))
						success = true;
				}
				control.unlock();

				//If the packet sent successfully, break for next packet
				if (success) 
					break;
			}
		}
	}

	//Close the socket now that we're done with it.
	//closesocket(socket_udp);
}