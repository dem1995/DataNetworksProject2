#pragma once
#pragma warning(disable: 4996)
#define _CRT_SECURE_NO_WARNINGS
#include <string>
#include <vector>
#include <iostream>
#include <sstream>

/// <summary>
/// Class for formatting headers (and holding packet data)
/// </summary>
class formatted_packet
{
public:
	/// <summary>
	/// Struct for holding formatted information about the ethernet header
	/// </summary>
	struct ethernet_header
	{
		std::string ether_destination;	//Bytes 0-5
		std::string ether_source;		//Bytes 6-11
		std::string ether_type;			//Bytes 12-13
		unsigned int packet_size{};		//Before ether section
		friend std::ostream& operator<<(std::ostream& os, const ethernet_header& dt)
		{
			os << "ETHER: ----- Ether Header-----" << std::endl;
			os << "\tETHER:" << std::endl;
			os << "\tETHER: Packet size : " << dt.packet_size << std::endl;
			os << "\tETHER: Destination : " << formatted_with_dashes(dt.ether_destination) << std::endl;
			os << "\tETHER: Source : " << formatted_with_dashes(dt.ether_source) << std::endl;
			os << "\tETHER: Ethertype : " << dt.ether_type << " (" << dt.get_ether_type_name() << ")" << std::endl;
			os << "\tETHER:";
			return os;
		}

		/// <summary>
		/// Formats the source/destination strings with dashes between every two characters
		/// (or any string, for that matter)
		/// </summary>
		static std::string formatted_with_dashes(std::string s)
		{
			std::string formatted_string;
			for (int i = 0; i < s.length(); i++)
			{
				formatted_string += s[i];
				if (i % 2 == 1 && i != s.length() - 1)
					formatted_string += "-";
			}
			return formatted_string;
		}

		/// <summary>
		/// Outputs IP, ARP, or UNKNOWN based on the ethertype bits
		/// </summary>
		std::string get_ether_type_name() const
		{
			if (ether_type == "0800" || ether_type == "86ee")
				return "IP";
			else if (ether_type == "0806")
				return "ARP";
			else
				return "UNKNOWN";
		}
	};

	/// <summary>
	/// Struct for holding formatted information about the ethernet header
	/// </summary>
	struct ip_header
	{
		std::string ip_version;				//First nybble of byte 14
		std::string ip_header_length;		//Second nybble of byte 14
		std::string type_of_service;		//Byte 15
		std::string total_length;			//Bytes 16-17
		std::string identification;			//Bytes 18-19
		std::string flags;					//Bytes 20-21
		std::string ttl;					//Byte 22
		std::string protocol;				//Byte 23
		std::string checksum;				//Bytes 24-25
		std::string source;					//Bytes 26-29
		std::string destination;			//Bytes 30-33

		friend std::ostream& operator<<(std::ostream& os, const ip_header& dt)
		{
			os << "IP: ----- IP Header-----" << std::endl;
			os << "\tIP:" << std::endl;
			os << "\tIP: Version = " << stoi(dt.ip_version, nullptr, 16) << std::endl;
			os << "\tIP: Header length = " << stoi(dt.ip_header_length, nullptr, 16) * 4 << std::endl;
			os << "\tIP: Type of service = 0x" << dt.type_of_service << std::endl;
			os << "\tIP: " << mask_and_keep(dt.type_of_service[0], 0xe) << ". .... = " << std::stoi(mask_and_keep(dt.type_of_service[0], 0x0e), nullptr, 2) << " (precedence)" << std::endl;
			os << "\tIP: ..." << mask_and_keep(dt.type_of_service[0], 0x1) << " .... = normal delay" << std::endl;
			os << "\tIP: .... " << mask_and_keep(dt.type_of_service[1], 0x8) << "... = normal throughput" << std::endl;
			os << "\tIP: .... ." << mask_and_keep(dt.type_of_service[1], 0x4) << ".. = normal reliability" << std::endl;
			os << "\tIP: Total length = " << stoi(dt.total_length, nullptr, 16) << std::endl;
			os << "\tIP: Identification = " << dt.identification << std::endl;
			os << "\tIP: Flags = 0x" << dt.flags << std::endl;
			os << "\tIP: ." << mask_and_keep(dt.flags[0], 0x4) << ".. .... .... .... = do not fragment" << std::endl;
			os << "\tIP: .." << mask_and_keep(dt.flags[0], 0x2) << ". .... .... .... = last fragment" << std::endl;
			std::string offset_flags = mask_and_keep(dt.flags[0], 0x1) + mask_and_keep(dt.flags[1], 0xf) + mask_and_keep(dt.flags[2], 0xf) + mask_and_keep(dt.flags[3], 0xf);
			os << "\tIP: ..." << offset_flags.substr(0, 1) << " " << offset_flags.substr(1, 4) << " " << offset_flags.substr(5, 4) << " " << offset_flags.substr(9, 4) << " = " << stoi(offset_flags, nullptr, 2) << " fragment offset" << std::endl;
			os << "\tIP: Fragment offset = " << stoi(offset_flags, nullptr, 2) << " bytes" << std::endl;
			os << "\tIP: Time to live = " << stoi(dt.ttl, nullptr, 16) << " seconds / hops" << std::endl;
			os << "\tIP: Protocol = " << stoi(dt.protocol, nullptr, 16) << " (" << dt.get_protocol_name() << ")" << std::endl;
			os << "\tIP: Header checksum = " << dt.checksum << std::endl;
			std::string source_address;
			source_address.append(std::to_string(stoi(dt.source.substr(0, 2), nullptr, 16)) + "." + std::to_string(stoi(dt.source.substr(2, 2), nullptr, 16)));
			source_address.append(("." + std::to_string(stoi(dt.source.substr(4, 2), nullptr, 16))));
			source_address.append(("." + std::to_string(stoi(dt.source.substr(6, 2), nullptr, 16))));

			os << "\tIP: Source address = " << source_address << std::endl;

			std::string destination_address;
			destination_address.append(std::to_string(stoi(dt.destination.substr(0, 2), nullptr, 16)) + "." + std::to_string(stoi(dt.destination.substr(2, 2), nullptr, 16)));
			destination_address.append(("." + std::to_string(stoi(dt.destination.substr(4, 2), nullptr, 16))));
			destination_address.append(("." + std::to_string(stoi(dt.destination.substr(6, 2), nullptr, 16))));
			os << "\tIP: Destination address = " << destination_address << std::endl;

			os << "\tIP: No options" << std::endl;
			os << "\tIP:";
			return os;
		}

		/// <summary>
		/// Masks the bits of s with the provided mask, then eliminates all characters not kept by the mask's rules
		/// and concatenates what's left
		/// </summary>
		static std::string mask_and_keep(const char s, const int mask)
		{
			int masked = (s&mask);
			std::string return_string;

			for (int i = 31; i >= 0; i--)
			{
				if (((mask >> i) & 0x01) == 1)
				{
					return_string += std::to_string(((masked >> i) & 0x01));
				}
			}
			return return_string;
		}

		/// <summary>
		/// Returns the protocol name (TCP, UDP, or UNKNOWN)
		/// </summary>
		std::string get_protocol_name() const
		{
			if (stoi(protocol, nullptr, 16) == 6)
				return "TCP";
			else if (stoi(protocol, nullptr, 16) == 17)
				return "UDP";
			else
				return "UNKNOWN";
		}
	};

	/// <summary>
	/// Struct for holding formatted information about the UDP header
	/// </summary>
	struct udp_header
	{
		std::string source_port;		//Bytes 34-35
		std::string destination_port;	//Bytes 36-37
		std::string length;				//Bytes 38-39
		std::string checksum;			//Bytes 40-41
		friend std::ostream& operator<<(std::ostream& os, const udp_header& dt)
		{
			return os;
		}
	};

	ethernet_header ethernet_header;
	ip_header ip_header;
	udp_header udp_header;
	std::vector<unsigned char> data;
	std::string hexData;

public:
	/// <summary>
	/// Populates the Formatted Header/data with an array of bytes
	/// </summary>
	formatted_packet(std::vector<unsigned char> packetData)
	{

		for (unsigned char c : packetData)
		{
			data.emplace_back(c);
		}

		hexData = "";
		for (int i = 0; i < packetData.size(); i++)
		{
			// Print each octet as hex (x), make sure there is always two characters (.2).
			char* formattedDataBuffer = new char[2];
			sprintf(formattedDataBuffer, "%.2x", data[i]);
			hexData.append(formattedDataBuffer);
		}
		const int offset = 4;
		ethernet_header = { hexData.substr(0,12), hexData.substr(12, 12), hexData.substr(24, 4), hexData.size()/2 };
		ip_header.ip_version = hexData.substr(28, 1);
		ip_header.ip_header_length = hexData.substr(29, 1);
		ip_header.type_of_service = hexData.substr(30 + offset, 2);
		ip_header.total_length = hexData.substr(32 + offset, 4);
		ip_header.identification = hexData.substr(36 + offset, 4);
		ip_header.flags = hexData.substr(40 + offset, 4);
		ip_header.ttl = hexData.substr(44 + offset, 2);
		ip_header.protocol = hexData.substr(46 + offset, 2);
		ip_header.checksum = hexData.substr(48 + offset, 4);
		ip_header.source = hexData.substr(52 + offset, 8);
		ip_header.destination = hexData.substr(60 + offset, 8);
		udp_header = { hexData.substr(68 + offset,4), hexData.substr(72 + offset,4), hexData.substr(76 + offset,4), hexData.substr(80 + offset,4) };
	}

	/// <summary>
	/// Populates the Formatted Header/data with a hexadecimal string
	/// </summary>
	explicit formatted_packet(const std::string hex_data_other)
	{
		hexData = "";
		hexData.append(hex_data_other);
		
		data = HexToBytes(hexData);

		const int offset = 4;
		ethernet_header = { hexData.substr(0,12), hexData.substr(12, 12), hexData.substr(24, 4), hexData.size()/2 };
		ip_header.ip_version = hexData.substr(28, 1);
		ip_header.ip_header_length = hexData.substr(29, 1);
		ip_header.type_of_service = hexData.substr(30 + offset, 2);
		ip_header.total_length = hexData.substr(32 + offset, 4);
		ip_header.identification = hexData.substr(36 + offset, 4);
		ip_header.flags = hexData.substr(40 + offset, 4);
		ip_header.ttl = hexData.substr(44 + offset, 2);
		ip_header.protocol = hexData.substr(46 + offset, 2);
		ip_header.checksum = hexData.substr(48 + offset, 4);
		ip_header.source = hexData.substr(52 + offset, 8);
		ip_header.destination = hexData.substr(60 + offset, 8);
		udp_header = { hexData.substr(68 + offset,4), hexData.substr(72 + offset,4), hexData.substr(76 + offset,4), hexData.substr(80 + offset,4) };
	}

	/// <summary>
	/// Converts a hex string to an array of bytes
	/// </summary>
	static std::vector<unsigned char> HexToBytes(const std::string& hex) {
		std::vector<unsigned char> bytes;

		for (unsigned int i = 0; i < hex.length(); i += 2) {
			std::string byteString = hex.substr(i, 2);
			unsigned char byte = (unsigned char)strtol(byteString.c_str(), NULL, 16);
			bytes.push_back(byte);
		}

		return bytes;
	}
	
	/// <summary>
	/// Overloaded stream insertion operation. Outputs information about headers for display.
	/// </summary>
	/// <param name="os">The stream to insert the header information into.</param>
	/// <param name="dt">The formatted packet to generate formatted text for display and insert into the stream.</param>
	/// <returns>A reference to the supplied ostream after the formatted packet information has been inserted.</returns>
	friend std::ostream& operator<<(std::ostream& os, const formatted_packet& dt)
	{
		//os << dt.ethernetHeader << std::endl;
		os << dt.ip_header << std::endl;

		for (int i = 0; i < dt.hexData.size(); i++)
		{
			if (i % 32 == 0)
			{
				if (i != 0 && i % 32 == 0)
					os << std::endl;
				std::string four_zeros = "0000";
				std::string val_string = std::to_string((i / 32) * 10);
				for (int i = 0; i < val_string.size() && i < four_zeros.size(); i++)
				{
					four_zeros[four_zeros.size() - 1 - i] = val_string[val_string.size() - 1 - i];
				}	
				os << four_zeros;
				os << " ";
				os << dt.hexData[i];
			}
			else
				os << dt.hexData[i];

			if (i % 2 == 1 && i != 0)
			{
				os << " ";
			}
		}
		os << std::endl;

		//os << str(dt.data.begin(), dt.data.end());

		os << std::endl;
		return os;
	}
	
	/// <summary>
	/// Reformats the hexadecimal address string into a period-separated decimal IP string.
	/// </summary>
	/// <param name="in">The hexadecimal address string</param>
	/// <returns>Returns a period-separated decimal IP address representation of the provided hexadecimal string.</returns>
	static std::string hexadecimal_to_decimal_ip(const std::string& in)
	{
		const char* array_in = in.c_str();
		char* out = (char*)malloc(sizeof(char) * 16);
		unsigned int p, q, r, s;

		if (sscanf(array_in, "%2x%2x%2x%2x", &p, &q, &r, &s) != 4)
			return out;
		sprintf(out, "%u.%u.%u.%u", p, q, r, s);
		return out;
	}
};



