#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <string>
#include <vector>
#include <cstdio>
#include <pcap.h>
#include "FormattedPacket.h"

/**
 * \brief Reads in packets from a packet capture file and formats them into FormattedPackets.
 * Returns a vector of the packets that were formatted.
 * \param file The file location of the capture file.
 * \return A vector of FormattedPackets formed from packet capture file at the provided location.
 */
inline std::vector<FormattedPacket> read_packet(const std::string& file)
	{

		// FormattedHeaders to be populated from the packets residing in the given capture file
		std::vector<FormattedPacket> formatted_headers;

		// Error buffer
		char errbuff[PCAP_ERRBUF_SIZE];

		// Open the pcap file, sticking error data into the error buffer
		pcap_t * pcap = pcap_open_offline(file.c_str(), errbuff);
		// http://www.winpcap.org/docs/docs_40_2/html/structpcap__pkthdr.html
		struct pcap_pkthdr* header;
		const u_char* data;

		// While packets can be retrieved, do so, dumping header information into the header pointer
		for (u_int packetCount = 0; pcap_next_ex(pcap, &header, &data) >= 0; packetCount++)
		{
			// Warn if the captured length is off
			if (header->len != header->caplen)
				printf("Warning! Capture size different than packet size: %ld bytes\n", header->len);

			//Reset the formatted string of data and formatted vector of data
			std::string formatted;
			auto packetData = std::vector<unsigned char>();

			// Loop through the packet and print it as hexadecimal representations of octets
			for (u_int i = 0; i < header->caplen; i++)
			{
				// Print each octet as hex (x), make sure there is always two characters (.2).
				char* formattedDataBuffer = new char[2];
				sprintf(formattedDataBuffer, "%.2x", data[i]);
				formatted.append(formattedDataBuffer);
				packetData.emplace_back(data[i]);
			}

			//Format the packet data based on the hex data and put it into the return vector
			formatted_headers.emplace_back(FormattedPacket(packetData));
		}
		return formatted_headers;
};