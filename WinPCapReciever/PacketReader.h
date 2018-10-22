#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <string>
#include <pcap.h>
#include <vector>
#include <cstdio>
#include "FormattedPacket.h"

inline std::vector<FormattedPacket> read_packet(std::string file)
	{
		std::vector<FormattedPacket> formattedHeaders;

		//Error buffer
		char errbuff[PCAP_ERRBUF_SIZE];

		//Open the pcap file, sticking error data into the error buffer
		pcap_t * pcap = pcap_open_offline(file.c_str(), errbuff);

		/*
		* Step 5 - Create a header and a data objects
		*/

		// Create a header object:
		// http://www.winpcap.org/docs/docs_40_2/html/structpcap__pkthdr.html
		struct pcap_pkthdr* header;

		// Array of bytes
		const u_char* data;

		/*
		* While packets can be retrieved, do so, dumping header information into the header pointer
		*/
		for (u_int packetCount = 0; pcap_next_ex(pcap, &header, &data) >= 0; packetCount++)
		{
			// Show a warning if the length captured is different
			if (header->len != header->caplen)
				printf("Warning! Capture size different than packet size: %ld bytes\n", header->len);

			//Reset the formatted string of data and formatted vector of data
			std::string formatted;
			auto packetData = std::vector<unsigned char>();


			// loop through the packet and print it as hexadecimal representations of octets
			// We also have a function that does this similarly below: PrintData()
			for (u_int i = 0; i < header->caplen; i++)
			{
				// Print each octet as hex (x), make sure there is always two characters (.2).
				char* formattedDataBuffer = new char[2];
				sprintf(formattedDataBuffer, "%.2x", data[i]);
				formatted.append(formattedDataBuffer);
				packetData.emplace_back(data[i]);
			}

			formattedHeaders.emplace_back(FormattedPacket(packetData));
		}
		return formattedHeaders;
};