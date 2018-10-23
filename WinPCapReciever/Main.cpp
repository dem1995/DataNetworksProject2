#pragma comment(lib, "Ws2_32.lib")
#include <thread>
#include <iostream>
#include <fstream>
#include "ThreadFunctions.h"
using namespace std;

/**
 * \brief Main function for Project 3 for Data Structures
 * \param argc The number of arguments
 * \param argv The (within the local folder Hosts) location of the text file specifying the local IP and port, as well
 * as neighbors
 * \return 0 if all went well, otherwise some other number
 */
int main(int argc, char *argv[])
{	
	//Prompt the user for input to provide a "pause" prior to setting things up and beginning to receive files (to satisfy assignment specs)
	{
		cout << "Press x, then \"enter\" to begin program flow up through receiving files." << endl;
		char c;
		cin >> c;
	}

	//Parse the network data and the pcap file location from the command parameters
	ifstream network_data;
	string pcap_file_location;

	if (argc <= 1)
	{
		return 1;
	}
	if (argc > 1)
	{
		//Parse the text file
		network_data = ifstream("Hosts/" + (string)argv[1]);
		if (!network_data)
		{
			cout << "Error. Input file not found" << endl;
			return 1;
		}
	}
	if (argc > 2)
	{
		//Determine the packet capture file location
		pcap_file_location = (string)argv[2];
	}

	//Get the local IP address
	network_data >> std::ws;
	string local_ip;
	network_data >> local_ip;
	cout << "The local IP is " << local_ip << std::endl;

	//Get the port number
	network_data >> std::ws;
	int local_port;
	network_data >> local_port;
	cout << "The local port is " << local_port << std::endl;

	//Grab the node's adjacent neighbors
	int num_neighbors;
	network_data >> num_neighbors;
	cout << "The number of neighbors is " << num_neighbors << std::endl;

	vector<string> neighbor_ips;
	vector<int> neighbor_ports;

	//Add each of the neighbors to this node
	for (int i = 0; i<num_neighbors; i++)
	{
		string neighbor_ip;
		network_data >> neighbor_ip;

		string temp_ip;
		network_data >> temp_ip;

		int neighbor_port;
		network_data >> neighbor_port;

		neighbor_ips.emplace_back(neighbor_ip);
		neighbor_ports.emplace_back(neighbor_port);
	}


	for (int i=0; i<num_neighbors; i++)
	{
		std::cout << neighbor_ips[i] << ": " << neighbor_ports[i] << std::endl;
	}

	//Create and dispatch the thread for receiving packets.
	thread packet_reception_thread = thread(receive_packets, local_ip, local_port);


	// Upon user prompt, create and dispatch the thread for sending packets.
	{
		cout << "Press x, then \"enter\" to execute packet sending." << endl;
		char c;
		cin >> c;
	}
	thread packet_sending_thread = thread(send_packets, pcap_file_location, local_ip, neighbor_ips, neighbor_ports);

	packet_sending_thread.join();
	packet_reception_thread.join();
	std::cout << "Exiting the program..." << std::endl;
	return 0;
}