#pragma comment(lib, "Ws2_32.lib")
#include <thread>
#include <iostream>
#include <fstream>
#include "ThreadFunctions.h"

using namespace std;

int main(int argc, char *argv[])
{	
	if (argc > 1)
	{
		//Parse the text file
		ofstream network_data("Hosts/" + (string)argv[1]);
		if (!network_data)
			cout << "Error" << endl;
	}

	// Create and dispatch the threads for receiving and sending the packets.
	thread packet_reception_thread(receive_packets, 3141);
	thread packet_sending_thread(send_packets, 3141);
	packet_sending_thread.join();
	packet_reception_thread.join();
	
	return 0;
}