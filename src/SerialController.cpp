#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>

#include <cstring>
#include <sstream>
#include <stdexcept>
#include <chrono>
#include <thread>

#ifndef WIN32
#include <unistd.h>
#include <sys/ioctl.h>
#else
#include <corecrt_io.h>
#endif

#include <iostream>

#include <primitives/Logger.h>

#include "SerialController.h"

SerialController::SerialController(const std::string& deviceFile, const int baudRate):
	m_deviceFile(deviceFile),
	m_baudRate(baudRate),
	m_connected(false),
	m_fileDescriptor(-1)
{
	connect();
}

SerialController::~SerialController()
{
	if(m_connected)
		close(m_fileDescriptor);
}

void SerialController::connect()
{
	m_fileDescriptor = open(m_deviceFile.c_str(), O_RDWR | O_NOCTTY | O_NDELAY);

	if (m_fileDescriptor == -1)
	{
		std::stringstream stream;
		stream << "Can't open " << m_deviceFile << ": ";
		stream << strerror(errno);
		//TODO Log
		return;
	}

	struct termios serial;
 
    if (tcgetattr(m_fileDescriptor, &serial) < 0) 
    {
        std::stringstream stream;
		stream << "Failed to read " << m_deviceFile << " parameters: ";
		stream << strerror(errno);
		
		//TODO Log
		return;
    }
    // Set up Serial Configuration
    serial.c_iflag = 0;
    serial.c_oflag = 0;
    serial.c_lflag = 0;
    serial.c_cflag = 0;
 
    serial.c_cc[VMIN] = 1;
    serial.c_cc[VTIME] = 1;
 	
 	speed_t baudRate = getRate(m_baudRate);

    serial.c_cflag = baudRate | CS8 | CREAD;

 	// Apply configuration
    tcsetattr(m_fileDescriptor, TCSANOW, &serial) < 0;

    m_connected = true;
}

int SerialController::serialDataAvail(const int fileDescriptor)
{
	int result;
	if (ioctl (fileDescriptor, FIONREAD, &result) == -1)
    	return -1;
	return result;
}

std::vector<uint8_t> SerialController::ReadMessage()
{
	// The delays required for the accumulation system input buffer

	if (!m_connected)
	{
		LOG_ERROR << "Failed to read message. Device is not open";
		return {};
	}

	int availableData = serialDataAvail(m_fileDescriptor);

	if (availableData == 0)
	{
		//std::this_thread::sleep_for (std::chrono::milliseconds(500));
		return std::vector<uint8_t>();
	}

	uint8_t buff[256];
	
	size_t readedBytes = read(m_fileDescriptor, buff, availableData);

    if (readedBytes < 0 || readedBytes == -1) 
    {
		//std::this_thread::sleep_for (std::chrono::milliseconds(500));
		return std::vector<uint8_t>();
    }

    //std::this_thread::sleep_for (std::chrono::milliseconds(500));

    //tcflush (m_fileDescriptor, TCIOFLUSH);

    std::vector<uint8_t> message;
	std::copy(std::begin(buff), std::begin(buff) + readedBytes, std::back_inserter(message));
	return message;
}

speed_t SerialController::getRate(int rate)
{
	speed_t myBaud ;

	switch (rate)
	{
		case      50:	myBaud =      B50 ; break ;
		case      75:	myBaud =      B75 ; break ;
		case     110:	myBaud =     B110 ; break ;
		case     134:	myBaud =     B134 ; break ;
		case     150:	myBaud =     B150 ; break ;
		case     200:	myBaud =     B200 ; break ;
		case     300:	myBaud =     B300 ; break ;
		case     600:	myBaud =     B600 ; break ;
		case    1200:	myBaud =    B1200 ; break ;
		case    1800:	myBaud =    B1800 ; break ;
		case    2400:	myBaud =    B2400 ; break ;
		case    4800:	myBaud =    B4800 ; break ;
		case    9600:	myBaud =    B9600 ; break ;
		case   19200:	myBaud =   B19200 ; break ;
		case   38400:	myBaud =   B38400 ; break ;
		case   57600:	myBaud =   B57600 ; break ;
		case  115200:	myBaud =  B115200 ; break ;
		case  230400:	myBaud =  B230400 ; break ;
		case  460800:	myBaud =  B460800 ; break ;
		case  500000:	myBaud =  B500000 ; break ;
		case  576000:	myBaud =  B576000 ; break ;
		case  921600:	myBaud =  B921600 ; break ;
		case 1000000:	myBaud = B1000000 ; break ;
		case 1152000:	myBaud = B1152000 ; break ;
		case 1500000:	myBaud = B1500000 ; break ;
		case 2000000:	myBaud = B2000000 ; break ;
		case 2500000:	myBaud = B2500000 ; break ;
		case 3000000:	myBaud = B3000000 ; break ;
		case 3500000:	myBaud = B3500000 ; break ;
		case 4000000:   myBaud = B4000000 ; break ;

		default:
			return B9600;
		break;
	}

	return myBaud;
}