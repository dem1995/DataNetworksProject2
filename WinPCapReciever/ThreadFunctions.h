#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <string>
#include <vector>
#include <winsock2.h>
#include "FormattedPacket.h"
#include "PacketReader.h"
#include <chrono>
#include <mutex>


static std::mutex control;

/**
 * \brief Function to receive packets set to the provided IP Address
 * \param local_ip The IP Address for which this function should receive packets
 * \param port_number the port number through which this function should receive packets
 */
inline void receive_packets(const std::string local_ip, const int port_number)
{
	//Preliminary setup for winsocket
	WSADATA wsa_data;
	WSAStartup(MAKEWORD(2, 2), &wsa_data);	////Decision to use MAKEWORD obtained from comment at https://docs.microsoft.com/en-us/windows/desktop/winsock/initializing-winsock

	//Prepare the socket we're binding to
	const int socket_udp = socket(AF_INET, SOCK_DGRAM, 0);

	//Read the retrieved packet captures into my FormattedPacket (and data) data structures
	auto packets = std::vector<formatted_packet>();
	struct sockaddr_in local_info {};
	struct sockaddr_in server_info {};
	local_info.sin_family = AF_INET;
	local_info.sin_port = htons(port_number);
	local_info.sin_addr.s_addr = INADDR_ANY;

	//Bind the socket
	bind(socket_udp, (sockaddr*)&local_info, sizeof(local_info));

	//Await transmission
	control.lock();
	std::cout << "Awaiting Transmission over port " << port_number << "...\n";
	control.unlock();
	while (true)
	{
		//Create and clear an array to hold the response
		char response[1024];
		std::fill(response, response + sizeof(response), 0);

		//Retrieve responses
		int server_info_length = sizeof(server_info);

		//If we receive a transmission
		if (recvfrom(socket_udp, response, sizeof(response), 0, (sockaddr*)&server_info, &server_info_length) != SOCKET_ERROR)
		{
			formatted_packet formattedResponse = formatted_packet(response);

			//If we are the destination IP for the transmission
			if (local_ip == formatted_packet::hexadecimal_to_decimal_ip(formattedResponse.ip_header.destination))
			{
				//Print the transmission and where we got it, then proceed to send an affirming response
				control.lock();
				{
					printf("Transmission from address %s received: %s\n", inet_ntoa(server_info.sin_addr), response);
					packets.emplace_back(formattedResponse);
					std::cout << packets[packets.size() - 1] << std::endl;
				}
				control.unlock();
				sendto(socket_udp, response, sizeof(response), 0, (sockaddr*)&server_info, server_info_length);
			}
		}

	}
}

/**
 * \brief Sends a given packet to a provided socket
 * \param reception_socket The socket to which to send the packet
 * \param packet The packet to be sent
 * \return true if the packet is successfully sent; false, otherwise.
 */
inline bool send_packet(sockaddr_in reception_socket, formatted_packet packet)
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
	//If we successfully send the packet, then close the socket and confirm its transmission

	bool sending_success = sendto(socket_udp, transmissionData, transmissionSize, 0, (sockaddr*)&reception_socket, serverInfoSize) != SOCKET_ERROR;
	closesocket(socket_udp);
	return sending_success;
	//if (sending_success)
	//{
	//	closesocket(socket_udp);
	//	////And successfully retrieve the packet
	//	//if (recvfrom(socket_udp, response, sizeof(response), 0, (sockaddr*)&reception_socket, &serverInfoSize) != SOCKET_ERROR)
	//	//{
	//	//	//Then break, allowing for the sending of the next packet.
	//	//	printf("Response received: %s\n", response);
	//	//	return true;
	//	//}
	//	return true;
	//}
	//closesocket(socket_udp);
	//return false;
}

/**
 * \brief Sends packets form the provided packet capture location that have the provided local ip address as their origin.
 * The packets are sent to the sockets specified by the provided neighbor IP addresses and neighbor ports.
 * \param packet_capture_location The file location of the packet captures to transmit
 * \param local_ip The local IP address whence transmitted packets should have their labelled origin 
 * \param neighbor_ips The ip address list portion of the reception sockets
 * \param neighbor_ports The port list portion of the reception sockets
 */
inline void send_packets(const std::string packet_capture_location, const std::string local_ip,
                         std::vector<std::string> neighbor_ips, std::vector<int> neighbor_ports)
{
	//Preliminary setup for winsocket
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	//Read the PCAP packet captures into my FormattedPacket (and data) data structures
	std::vector<formatted_packet> packets = read_packet(packet_capture_location);

	//	std::vector<formatted_packet> packets = read_packet(
	//R"(C:\Users\DEMcKnight\source\repos\WPD2\WinPCapReciever\Packets\Project2Topo.pcap)");

	int counter = 0;
	//For each packet, send its contents over the UDP connection
	for (formatted_packet packet : packets)
	{
		counter++;
		//If the packet's source matches our own, we send it onwards
		if (formatted_packet::hexadecimal_to_decimal_ip(packet.ip_header.source) == local_ip)
		{			

			//Send the packet to each of this node's neighbors
			control.lock();
			{
				std::cout << "Sending packet number" << counter << ":..." << std::endl;
				for (int i = 0; i < neighbor_ips.size(); i++)
				{
					struct sockaddr_in server_info {};
					server_info.sin_family = AF_INET;
					server_info.sin_port = htons(neighbor_ports[i]);
					server_info.sin_addr.s_addr = inet_addr("127.3.1.4");
					send_packet(server_info, packet);
					
				}
			}
			control.unlock();


			//auto start = std::chrono::steady_clock::now();
			//int since_printed = 0;
			//while (true)
			//{
			//	//Keeps track of how long we've run to make sure we don't try too many times with this packet
			//	int time_passed = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - start).count();

			//	//Breaks the loop if the time limit is exceeded
			//	bool breakLoop = false;
			//	control.lock();
			//	{
			//		//If the packet sent successfully, break for next packet
			//		if (send_packet(server_info, packet))
			//		{
			//			std::cout << "Sending succeeded!" << std::endl;
			//			breakLoop = true;
			//		}
			//		//If we take too long, the sending failed.
			//		else if (time_passed > 10000)
			//		{
			//			std::cout << "Sending failed..." << std::endl;
			//			breakLoop = true;
			//		}
			//	}
			//	control.unlock();
			//	if (breakLoop)
			//		break;
			//}
		}
	}
}