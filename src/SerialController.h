#ifndef SERIAL_CONTROLLER
#define SERIAL_CONTROLLER

#include <string>
#include <vector>
#include <algorithm>
#include <memory>

#include <termios.h>

class SerialController;
typedef std::shared_ptr<SerialController> SerialControllerP;

// not singleton anymore
class SerialController
{
	SerialController() = delete;
	SerialController(const SerialController&) = delete;
	SerialController& operator=(SerialController) = delete;

public:

	SerialController(const std::string& deviceFile, const int baudRate);
	~SerialController();

	
	static SerialControllerP CreateInstance()
	{
		std::string dev = "/dev/ttyS0";
		auto instance = std::make_shared<SerialController>(dev, 9600);
		return instance;
	}

	void connect();
	std::vector<uint8_t> ReadMessage();

	private:
	speed_t getRate(int rate);
	int serialDataAvail(const int fileDescriptor);

	private:
	std::string m_deviceFile;
	int m_baudRate;
	bool m_connected;
	int m_fileDescriptor;
};

#endif