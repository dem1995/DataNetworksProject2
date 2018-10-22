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
class FormattedPacket
{
public:
	/// <summary>
	/// Class for holding formatted information about the ethernet header
	/// </summary>
	struct EthernetHeader
	{
		std::string etherDestination;	//Bytes 0-5
		std::string etherSource;			//Bytes 6-11
		std::string etherType;			//Bytes 12-13
		unsigned int packetSize;	//Before ether section
		friend std::ostream& operator<<(std::ostream& os, const EthernetHeader& dt)
		{
			os << "ETHER: ----- Ether Header-----" << std::endl;
			os << "\tETHER:" << std::endl;
			os << "\tETHER: Packet size : " << dt.packetSize << std::endl;
			os << "\tETHER: Destination : " << formattedWithDashes(dt.etherDestination) << std::endl;
			os << "\tETHER: Source : " << formattedWithDashes(dt.etherSource) << std::endl;
			os << "\tETHER: Ethertype : " << dt.etherType << " (" << dt.getEtherTypeName() << ")" << std::endl;
			os << "\tETHER:";
			return os;
		}

		/// <summary>
		/// Formats the source/destination strings with dashes between every two characters
		/// (or any string, for that matter)
		/// </summary>
		static std::string formattedWithDashes(std::string s)
		{
			std::string formattedString;
			for (int i = 0; i < s.length(); i++)
			{
				formattedString += s[i];
				if (i % 2 == 1 && i != s.length() - 1)
					formattedString += "-";
			}
			return formattedString;
		}

		/// <summary>
		/// Outputs IP, ARP, or UNKNOWN based on the ethertype bits
		/// </summary>
		std::string getEtherTypeName() const
		{
			if (etherType == "0800" || etherType == "86ee")
				return "IP";
			else if (etherType == "0806")
				return "ARP";
			else
				return "UNKNOWN";
		}
	};

	/// <summary>
	/// Class for holding formatted information about the ethernet header
	/// </summary>
	struct IPHeader
	{
		std::string ipVersion;			//First nybble of byte 14
		std::string ipHeaderLength;		//Second nybble of byte 14
		std::string typeOfService;		//Byte 15
		std::string totalLength;			//Bytes 16-17
		std::string identification;		//Bytes 18-19
		std::string flags;				//Bytes 20-21
		std::string ttl;					//Byte 22
		std::string protocol;			//Byte 23
		std::string checksum;			//Bytes 24-25
		std::string source;				//Bytes 26-29
		std::string destination;			//Bytes 30-33

		friend std::ostream& operator<<(std::ostream& os, const IPHeader& dt)
		{
			os << "IP: ----- IP Header-----" << std::endl;
			os << "\tIP:" << std::endl;
			os << "\tIP: Version = " << stoi(dt.ipVersion, nullptr, 16) << std::endl;
			os << "\tIP: Header length = " << stoi(dt.ipHeaderLength, nullptr, 16) * 4 << std::endl;
			os << "\tIP: Type of service = 0x" << dt.typeOfService << std::endl;
			os << "\tIP: " << maskAndKeep(dt.typeOfService[0], 0xe) << ". .... = " << std::stoi(maskAndKeep(dt.typeOfService[0], 0x0e), nullptr, 2) << " (precedence)" << std::endl;
			os << "\tIP: ..." << maskAndKeep(dt.typeOfService[0], 0x1) << " .... = normal delay" << std::endl;
			os << "\tIP: .... " << maskAndKeep(dt.typeOfService[1], 0x8) << "... = normal throughput" << std::endl;
			os << "\tIP: .... ." << maskAndKeep(dt.typeOfService[1], 0x4) << ".. = normal reliability" << std::endl;
			os << "\tIP: Total length = " << stoi(dt.totalLength, nullptr, 16) << std::endl;
			os << "\tIP: Identification = " << dt.identification << std::endl;
			os << "\tIP: Flags = 0x" << dt.flags << std::endl;
			os << "\tIP: ." << maskAndKeep(dt.flags[0], 0x4) << ".. .... .... .... = do not fragment" << std::endl;
			os << "\tIP: .." << maskAndKeep(dt.flags[0], 0x2) << ". .... .... .... = last fragment" << std::endl;
			std::string offsetFlags = maskAndKeep(dt.flags[0], 0x1) + maskAndKeep(dt.flags[1], 0xf) + maskAndKeep(dt.flags[2], 0xf) + maskAndKeep(dt.flags[3], 0xf);
			os << "\tIP: ..." << offsetFlags.substr(0, 1) << " " << offsetFlags.substr(1, 4) << " " << offsetFlags.substr(5, 4) << " " << offsetFlags.substr(9, 4) << " = " << stoi(offsetFlags, nullptr, 2) << " fragment offset" << std::endl;
			os << "\tIP: Fragment offset = " << stoi(offsetFlags, nullptr, 2) << " bytes" << std::endl;
			os << "\tIP: Time to live = " << stoi(dt.ttl, nullptr, 16) << " seconds / hops" << std::endl;
			os << "\tIP: Protocol = " << stoi(dt.protocol, nullptr, 16) << " (" << dt.getProtocolName() << ")" << std::endl;
			os << "\tIP: Header checksum = " << dt.checksum << std::endl;
			std::string sourceAddress = "";
			sourceAddress.append(std::to_string(stoi(dt.source.substr(0, 2), nullptr, 16)) + "." + std::to_string(stoi(dt.source.substr(2, 2), nullptr, 16)));
			sourceAddress.append(("." + std::to_string(stoi(dt.source.substr(4, 2), nullptr, 16))));
			sourceAddress.append(("." + std::to_string(stoi(dt.source.substr(6, 2), nullptr, 16))));

			os << "\tIP: Source address = " << sourceAddress << std::endl;

			std::string destinationAddress = "";
			destinationAddress.append(std::to_string(stoi(dt.destination.substr(0, 2), nullptr, 16)) + "." + std::to_string(stoi(dt.destination.substr(2, 2), nullptr, 16)));
			destinationAddress.append(("." + std::to_string(stoi(dt.destination.substr(4, 2), nullptr, 16))));
			destinationAddress.append(("." + std::to_string(stoi(dt.destination.substr(6, 2), nullptr, 16))));
			os << "\tIP: Destination address = " << destinationAddress << std::endl;

			os << "\tIP: No options" << std::endl;
			os << "\tIP:";
			return os;
		}

		/// <summary>
		/// Masks the bits of s with the provided mask, then eliminates all characters not kept by the mask's rules
		/// and concatenates what's left
		/// </summary>
		static std::string maskAndKeep(char s, int mask)
		{
			int masked = (s&mask);
			std::string returnString = "";

			for (int i = 31; i >= 0; i--)
			{
				if (((mask >> i) & 0x01) == 1)
				{
					returnString += std::to_string(((masked >> i) & 0x01));
				}
			}
			return returnString;
		}

		/// <summary>
		/// Returns the protocol name (TCP, UDP, or UNKNOWN)
		/// </summary>
		std::string getProtocolName() const
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
	/// Class for holding formatted information about the UDP header
	/// </summary>
	struct UDPHeader
	{
		std::string sourcePort;			//Bytes 34-35
		std::string destinationPort;		//Bytes 36-37
		std::string length;				//Bytes 38-39
		std::string checksum;			//Bytes 40-41
		friend std::ostream& operator<<(std::ostream& os, const UDPHeader& dt)
		{
			return os;
		}
	};

	EthernetHeader ethernetHeader;
	IPHeader ipHeader;
	UDPHeader udpHeader;
	std::vector<unsigned char> data;
	std::string hexData;

public:
	/// <summary>
	/// Populates the Formatted Header/data with an array of bytes
	/// </summary>
	FormattedPacket(std::vector<unsigned char> packetData)
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

		ethernetHeader = { hexData.substr(0,12), hexData.substr(12, 12), hexData.substr(24, 4), hexData.size()/2 };
		ipHeader = IPHeader();
		ipHeader.ipVersion = hexData.substr(28, 1);
		ipHeader.ipHeaderLength = hexData.substr(29, 1);
		ipHeader.typeOfService = hexData.substr(30, 2);
		ipHeader.totalLength = hexData.substr(32, 4);
		ipHeader.identification = hexData.substr(36, 4);
		ipHeader.flags = hexData.substr(40, 4);
		ipHeader.ttl = hexData.substr(44, 2);
		ipHeader.protocol = hexData.substr(46, 2);
		ipHeader.checksum = hexData.substr(48, 4);
		ipHeader.source = hexData.substr(52, 8);
		ipHeader.destination = hexData.substr(60, 8);
		udpHeader = { hexData.substr(68,4), hexData.substr(72,4), hexData.substr(76,4), hexData.substr(80,4) };
	}

	/// <summary>
	/// Populates the Formatted Header/data with a hexadecimal string
	/// </summary>
	FormattedPacket(std::string hexDataOther)
	{

		hexData = "";
		hexData.append(hexDataOther);
		
		data = HexToBytes(hexData);

		ethernetHeader = { hexData.substr(0,12), hexData.substr(12, 12), hexData.substr(24, 4), hexData.size()/2 };
		ipHeader = IPHeader();
		ipHeader.ipVersion = hexData.substr(28, 1);
		ipHeader.ipHeaderLength = hexData.substr(29, 1);
		ipHeader.typeOfService = hexData.substr(30, 2);
		ipHeader.totalLength = hexData.substr(32, 4);
		ipHeader.identification = hexData.substr(36, 4);
		ipHeader.flags = hexData.substr(40, 4);
		ipHeader.ttl = hexData.substr(44, 2);
		ipHeader.protocol = hexData.substr(46, 2);
		ipHeader.checksum = hexData.substr(48, 4);
		ipHeader.source = hexData.substr(52, 8);
		ipHeader.destination = hexData.substr(60, 8);
		udpHeader = { hexData.substr(68,4), hexData.substr(72,4), hexData.substr(76,4), hexData.substr(80,4) };
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

	friend std::ostream& operator<<(std::ostream& os, const FormattedPacket& dt)
	{
		os << dt.ethernetHeader << std::endl;
		os << dt.ipHeader << std::endl;

		for (int i = 0; i < dt.hexData.size(); i++)
		{
			if (i % 32 == 0)
			{
				if (i != 0 && i % 32 == 0)
					os << std::endl;
				std::string fourZeros = "0000";
				std::string valString = std::to_string((i / 32) * 10);
				for (int i = 0; i < valString.size() && i < fourZeros.size(); i++)
				{
					fourZeros[fourZeros.size() - 1 - i] = valString[valString.size() - 1 - i];
				}	
				os << fourZeros;
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
};

