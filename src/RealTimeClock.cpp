#include <errno.h>
#include <stdlib.h>
#include <fcntl.h>
#include <cstring>

#include <primitives/Logger.h>

#ifndef WIN32
#include <unistd.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#else
#include <corecrt_io.h>
#endif

#include <sstream>
#include <stdexcept>

#include "RealTimeClock.h"

RealTimeClock::RealTimeClock(const std::string& deviceFile, uint16_t slaveAddress):
	m_connected(false),
	m_fileDescriptor(-1),
	m_deviceFile(deviceFile),
	m_slaveAddress(slaveAddress)
{
	connect();
}

RealTimeClock::~RealTimeClock()
{
	if (m_connected)
		close(m_fileDescriptor);
}

void RealTimeClock::connect()
{
	m_fileDescriptor = open(m_deviceFile.c_str(), O_RDWR);
	
	if (m_fileDescriptor == -1)
	{
		std::stringstream stream;
		stream << "Can't open " << m_deviceFile << ": ";
		stream << strerror(errno) << std::endl;
		LOG_ERROR << stream.str();
		return;
	}
	
#ifndef WIN32
	if (ioctl(m_fileDescriptor, I2C_SLAVE, m_slaveAddress) < 0)
	{
		std::stringstream stream;
		stream << "Can't connect to " << m_slaveAddress << ": ";
		stream << strerror(errno) << std::endl;
		LOG_ERROR << stream.str();
		return;
	}
#endif

	m_connected = true;
}

void RealTimeClock::SetTime(std::tm& time)
{
	std::lock_guard<std::mutex> lock(m_mutex);
	
	const size_t BUFFER_SIZE = 8;
	uint8_t buf[BUFFER_SIZE];
	
	// First buf element is start register position
	buf[0] = 0x00;
	
	buf[1] = convertToRegisterFormat(time.tm_sec);
	buf[2] = convertToRegisterFormat(time.tm_min);
	buf[3] = convertToRegisterFormat(time.tm_hour);
	buf[4] = convertToRegisterFormat(time.tm_wday + 1);
	buf[5] = convertToRegisterFormat(time.tm_mday);
	buf[6] = convertToRegisterFormat(time.tm_mon + 1);
	buf[7] = convertToRegisterFormat(time.tm_year - 100);
	
	if (write(m_fileDescriptor, buf, BUFFER_SIZE) != BUFFER_SIZE )
	{
		std::stringstream stream;
		stream << "Can't set time: " << strerror(errno) << std::endl;
		
		LOG_ERROR <<stream.str();
	}
}

std::tm RealTimeClock::GetTime()
{
	uint8_t position = 0x00;
	const size_t BUFFER_SIZE = 7;
	uint8_t buf[BUFFER_SIZE];
	
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		
		// Before read we need set start register position
		if (write(m_fileDescriptor, &position, 1) != 1)
		{
			std::stringstream stream;
			stream << "Can't write position: " << strerror(errno) << std::endl;
			
			LOG_ERROR << stream.str();
			return {};
		}
		
		if (read(m_fileDescriptor, buf, BUFFER_SIZE) != BUFFER_SIZE)
		{
			std::stringstream stream;
			stream << "Can't get position: " << strerror(errno) << std::endl;
		
			LOG_ERROR << stream.str();
			return {};
		}
	}

	std::tm time;
	
	time.tm_sec = convertFromRegisterFormat(buf[0]);
	time.tm_min = convertFromRegisterFormat(buf[1]);
	time.tm_hour = convertFromRegisterFormat(buf[2]);
	time.tm_wday = convertFromRegisterFormat(buf[3]) - 1;
	time.tm_mday = convertFromRegisterFormat(buf[4]);
	time.tm_mon = convertFromRegisterFormat(buf[5]) - 1;
	time.tm_year = 100 + convertFromRegisterFormat(buf[6]);
	
	// Summer time switch disabled
	time.tm_isdst = 0;
	
	return time;
}

uint8_t RealTimeClock::convertFromRegisterFormat(uint8_t input)
{
	/* Typical 8 bit register of DS3231
	 * left side (b7-b4) - is 10 part
	 * right side (b3-b0) - single digit
	 * Example:
	 * 25 sec
	 * [b7][b6][b5][b4] [b3][b2][b1][b0]
	 *       0010              0101     
	 *       2*10      +         5      
	 *                 25               
	 */
	uint8_t left = input >> 4;
	uint8_t right = input & 15;
	return left * 10 + right;
}

uint8_t RealTimeClock::convertToRegisterFormat(uint8_t input)
{
	//This operation is reverse convertFromRegisterFormat()
	uint8_t tenPart = input / 10;
	uint8_t singlePart = input - tenPart * 10;
	uint8_t result = tenPart << 4;
	
	return result | singlePart;
}

