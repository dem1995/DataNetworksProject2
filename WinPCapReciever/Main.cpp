#pragma comment(lib, "Ws2_32.lib")
#include <thread>
#include <iostream>
#include <fstream>
#include "ThreadFunctions.h"
#include "SenderMain.h"

using namespace std;

int main(int argc, char *argv[])
{	
	ifstream network_data;

	if (argc <= 1)
		return 1;

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

	// Create and dispatch the threads for receiving and sending the packets.
	//main2();
	//send_packets(local_ip, local_port, neighbor_ips, neighbor_ports);
	thread packet_sending_thread(send_packets, local_ip, local_port, neighbor_ips, neighbor_ports);
	thread packet_reception_thread(receive_packets, local_port);
	packet_sending_thread.join();
	packet_reception_thread.join();
	std::cout << "Press 'x', then 'enter'" << std::endl;
	std::string s;
	std::cin >> s;
	
	return 0;
}