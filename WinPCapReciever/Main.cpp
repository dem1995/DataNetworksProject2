#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma comment(lib, "Ws2_32.lib")

#include <winsock2.h>
#include <thread>
#include "ThreadFunctions.h"

#define MAX_THREADS 2
#define BUF_SIZE 255

DWORD WINAPI start_packet_reception_thread(LPVOID lpParam);
DWORD WINAPI start_packet_sending_thread(LPVOID lpParam);

using namespace std;


int main(int argc, char *argv[])
{	

	//The thread IDs for the reader and sender threads
	DWORD   readerThreadId;
	DWORD	senderThreadId;

	//Allocating space for the socket info
	auto pDataReader =
		static_cast<PMYDATA>(HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(MYDATA)));
	auto pDataSender = 
		static_cast<PMYDATA>(HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(MYDATA)));

	//pDataReader->address = "10.204.40.16";
	//pDataReader->port_number = 3141;

	thread packet_reception_thread(receive_packets, 3141);
	thread packet_sending_thread(send_packets, pDataReader->address, pDataReader->port_number);
	packet_reception_thread.join();
	
	//packet_reception_thread.join();


	//const HANDLE readerThreadHandle = CreateThread(
	//	nullptr, // default security attributes
	//	0, // use default stack size  
	//	start_packet_reception_thread, // thread function name
	//	pDataReader, // argument to thread function 
	//	0, // use default creation flags 
	//	&readerThreadId);		// returns the thread identifier 
	//const HANDLE senderThreadHandle = CreateThread(
	//	nullptr, // default security attributes
	//	0, // use default stack size  
	//	start_packet_sending_thread, // thread function name
	//	pDataSender, // argument to thread function 
	//	0, // use default creation flags 
	//	&senderThreadId);		// returns the thread identifier 

	//HANDLE activeThreads[2] = { readerThreadHandle, senderThreadHandle };
	//WaitForMultipleObjects(2, activeThreads, TRUE, INFINITE);

	//// Close all thread handles and free memory allocations.
	//for (auto& activeThread : activeThreads)
	//{
	//	CloseHandle(activeThread);
	//	if (pDataReader != nullptr)
	//	{
	//		HeapFree(GetProcessHeap(), 0, pDataReader);
	//		pDataReader = nullptr;    // Ensure address is not reused.
	//	}
	//}
	return 0;
}


DWORD WINAPI start_packet_reception_thread(LPVOID lpParam)
{
	// Cast the parameter to the correct data type.
	// The pointer is known to be valid because 
	// it was checked for NULL before the thread was created
	const auto local_socket  = static_cast<PMYDATA>(lpParam);
	//receive_packets(local_socket->address, local_socket->port_number);
	return 0;
}

DWORD WINAPI start_packet_sending_thread(LPVOID lpParam)
{	
	// Cast the parameter to the correct data type.
	// The pointer is known to be valid because 
	// it was checked for NULL before the thread was created
	const auto local_socket = static_cast<PMYDATA>(lpParam);
	//send_packets(local_socket->address, local_socket->port_number);
	return 0;
}



