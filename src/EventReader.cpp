#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>

#include <cstring>
#include <sstream>
#include <stdexcept>
#include <thread>
#include <iomanip>
////
#include <string>
#include <iostream>
#include <algorithm>
#include <numeric>
#include <vector>
#include <sstream>
////
#ifndef WIN32
#include "SerialController.h"
#endif

#include <primitives/Logger.h>

#include "EventReader.h"
#include "SoapHelpers.h"

#define LOW_BYTE_EXTRACT(toByteDigit) toByteDigit & 0b0000000011111111

// Packet common fileds position
#define START_BYTE 0
#define MSB 1
#define LSB 2
#define FRAME_TYPE 3
#define SRC_ID_MSB 4
#define SRC_ID_LSB 5

#define START_BYTE_VALUE 0x7e

// Device types and their messages
#define FRAME_DETECTOR 0x08
#define FD_METAL_DETECTED 0x01
#define FD_RAD_DETECTED 0x05
#define FD_EXPLOSIVE_DETECTED 0x07

#define EXPLOSIVE_DETECTOR 0x07
#define ED_EXPLOSIVE_DETECTED 0x01

#define HANDHELD_DETECTOR_1 0x1
#define HANDHELD_DETECTOR_2 0x2
#define HANDHELD_DETECTOR_3 0x3
#define HANDHELD_DETECTOR_4 0x4

inline bool checkHighestBitOfByte(uint8_t byte)
{
	// 1xxx xxxx
	// 000000001
	return byte >> 7;
}

std::string notificationToString(NotificationMessage& message)
{
	std::string messageType;
	switch(message.type)
	{
		case MessageType::MetallDetector:
		messageType = "Metall detector";
		break;

		case MessageType::SteamDetector:
		messageType = "Explosive detector";
		break;

		case MessageType::RadiationMonitoring:
		messageType = "Radiation monitoring";
		break;

		default:
		messageType = "";
	}

	std::stringstream stream;
	stream << messageType << " : " << message.Mesures << " : " << std::to_string(message.time.count());
	return stream.str();
}

uint16_t combine2Bytes(uint8_t high, uint8_t low)
{
	uint16_t result = 0;
	result = high;
	// Set high bits left
	result <<= 8;
	// Set low bits
	return result | low;
}

bool checkHashSumm(const std::vector<uint8_t>& message)
{
	// Ignore first 3 byte
	const uint16_t CHECKABLE_DATA_POSITION = 3;
	uint16_t checkSumm = std::accumulate(message.begin() + CHECKABLE_DATA_POSITION, message.end(), 0);

	return (LOW_BYTE_EXTRACT(checkSumm)) == 0xff;
}

uint8_t calcHashSumm(const std::vector<uint8_t>& message)
{
	// Ignore first 3 byte and last byte
	const uint16_t CHECKABLE_DATA_POSITION = 3;
	uint16_t checkSumm = std::accumulate(message.begin() + CHECKABLE_DATA_POSITION, message.end() - 1, 0);
	//const uint16_t LOW_BYTE_EXTRACT = 0b0000000011111111;

	return 0xff - LOW_BYTE_EXTRACT(checkSumm);
}

void parseUartPacket(const std::vector<uint8_t>& mes, std::vector<NotificationMessage>& result)
{
	const uint8_t frameType = mes[FRAME_TYPE];
	uint8_t dataStartPos = 0;

	switch (frameType)
	{
	case 0x8f:
		dataStartPos = 8;
		break;

	case 0x83:
		dataStartPos = 10;
		break;

	default:
		LOG_ERROR << "Unknow frame type " << static_cast<uint16_t>(frameType);
		return;
	}

	uint16_t sourceID = combine2Bytes(mes[SRC_ID_MSB], mes[SRC_ID_LSB]);

	uint8_t comandCode = mes[dataStartPos];

	// Ignore command id byte
	auto dataBuff = mes.begin() + dataStartPos + 1;

	std::stringstream resultMessge;

	if (sourceID == FRAME_DETECTOR)
	{
		switch (comandCode)
		{
		case FD_METAL_DETECTED:
		{
			resultMessge << "Metal detected";
			NotificationMessage messageMetall = { MessageType::MetallDetector, "", std::chrono::milliseconds(SoapHelpers::getCurrentTime()) };
			result.push_back(messageMetall);
			LOG_INFO << notificationToString(messageMetall);
			return;
		}
		case FD_EXPLOSIVE_DETECTED:
		{
			resultMessge << (uint16_t)dataBuff[0] << ", " << (uint16_t)dataBuff[1] << ", " << (uint16_t)dataBuff[2] << ", " << (uint16_t)dataBuff[3];
			NotificationMessage messageExplosive = { MessageType::SteamDetector, resultMessge.str(), std::chrono::milliseconds(SoapHelpers::getCurrentTime())};
			result.push_back(messageExplosive);
			LOG_INFO << notificationToString(messageExplosive);
			return;
		}
		case FD_RAD_DETECTED:
		{
			resultMessge << "More than 0.3mSv / h";
			NotificationMessage messageRad = { MessageType::RadiationMonitoring, resultMessge.str(), std::chrono::milliseconds(SoapHelpers::getCurrentTime())};
			result.push_back(messageRad);
			LOG_INFO << notificationToString(messageRad);
			return;
		}
		default:
			LOG_ERROR << "Unknow frame detecor message type " << static_cast<uint16_t>(comandCode);
			return;
		}
	}
	if (sourceID == EXPLOSIVE_DETECTOR)
	{
		switch (comandCode)
		{
		case ED_EXPLOSIVE_DETECTED:
		{
			std::cout << "Im here" << std::endl;
			resultMessge << (uint16_t)dataBuff[0] << ", " << (uint16_t)dataBuff[1] << ", " << combine2Bytes(dataBuff[2], dataBuff[3]) << ", ";
			resultMessge << combine2Bytes(dataBuff[4], dataBuff[5]) << ", " << (uint16_t)dataBuff[6] << ", " << (uint16_t)dataBuff[7] << ", ";
			resultMessge << combine2Bytes(dataBuff[8], dataBuff[9]) << ", " << combine2Bytes(dataBuff[10], dataBuff[11]) << ", ";
			resultMessge << (uint16_t)dataBuff[12] << ", " << (uint16_t)dataBuff[13] << ", " << combine2Bytes(dataBuff[14], dataBuff[15]) << ", ";
			resultMessge << combine2Bytes(dataBuff[16], dataBuff[17]) << ", " << (uint16_t)dataBuff[18] << ", " << (uint16_t)dataBuff[19] << ", ";
			resultMessge << combine2Bytes(dataBuff[20], dataBuff[21]) << ", " << combine2Bytes(dataBuff[22], dataBuff[23]);
			NotificationMessage messageExplosive = { MessageType::SteamDetector, resultMessge.str(), std::chrono::milliseconds(SoapHelpers::getCurrentTime())};
			result.push_back(messageExplosive);
			LOG_INFO << notificationToString(messageExplosive);
			return;
		}
		default:
			LOG_ERROR << "Unknow explosive detecor message type " << static_cast<uint16_t>(comandCode);
			return;
		}
	}
	else if (sourceID == HANDHELD_DETECTOR_1 || sourceID == HANDHELD_DETECTOR_2
		|| sourceID == HANDHELD_DETECTOR_3 || sourceID == HANDHELD_DETECTOR_4)
	{
		if(checkHighestBitOfByte(dataBuff[0]))
		{
			resultMessge << "Metal detected";
			NotificationMessage messageMetall = { MessageType::MetallDetector, "", std::chrono::milliseconds(SoapHelpers::getCurrentTime()) };
			result.push_back(messageMetall);
			LOG_INFO << notificationToString(messageMetall);
		}

		if(checkHighestBitOfByte(dataBuff[2]))
		{
			resultMessge.str("");
			resultMessge << "More than 0.3microSv / h";
			NotificationMessage messageRad = { MessageType::RadiationMonitoring, resultMessge.str(), std::chrono::milliseconds(SoapHelpers::getCurrentTime())};
			result.push_back(messageRad);
			LOG_INFO << notificationToString(messageRad);
		}

		return;
	}
	else
	{
		LOG_ERROR << "Unknow source id message type " << static_cast<uint16_t>(sourceID);
		return;
	}

}


EventReader::EventReader()
{
#ifndef WIN32
	m_controller = SerialController::CreateInstance();
#endif
}

EventReader::~EventReader()
{

}


std::vector<NotificationMessage> EventReader::ReadEvents()
{
	std::vector<NotificationMessage> result;

#ifndef WIN32
	auto data = m_controller->ReadMessage();
#else

	std::vector<uint8_t> data;
	NotificationMessage messageMetall = { MessageType::MetallDetector, "", std::chrono::milliseconds(SoapHelpers::getCurrentTime()) };
	result.push_back(messageMetall);

	std::this_thread::sleep_for(std::chrono::seconds(1));

#endif

	if(data.empty())
	{
		return result;
	}

	std::stringstream stream;
	for(auto& value : data)
		stream << std::setfill('0') << std::setw(2) << std::hex << static_cast<int>(value) << " ";

	LOG_INFO << "Recived row data : " << stream.str();

	m_buffer.insert(m_buffer.end(), data.begin(), data.end());

	while (!m_buffer.empty())
	{
		auto it = std::find(m_buffer.begin(), m_buffer.end(), START_BYTE_VALUE);

		m_buffer.erase(m_buffer.begin(), it);

		if (m_buffer.size() <= 4)
		{
			return result;
		}
		auto& msbValue = *(++it);
		auto& lsbValue = *(++it);

		const uint16_t frameLength = combine2Bytes(msbValue, lsbValue);

		if (m_buffer.size() < frameLength + 4)
		{
			return result;
		}

		std::vector<uint8_t> uartPacket;
		auto end = m_buffer.begin();
		std::advance(end, frameLength + 4);
		std::copy(m_buffer.begin(), end, std::back_inserter(uartPacket));
		
		if(!checkHashSumm(uartPacket))
		{
			LOG_ERROR << "Control sum is wrong! Skip current START_BYTE_VALUE";

			m_buffer.erase(m_buffer.begin()); // Skip.
			continue;
		}
		m_buffer.erase(m_buffer.begin(), end);

		parseUartPacket(uartPacket, result);
		
	}
	return result;
}
