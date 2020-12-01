#ifndef GPIO_CLASS_H
#define GPIO_CLASS_H

#include <string>

class GPIOClass
{
public:
	GPIOClass();  // create a GPIO object that controls GPIO4
	GPIOClass(std::string port); // create a GPIO object that controls GPIOx, where x is passed to this constructor
	~GPIOClass();
	
	int Write(std::string val); // Set GPIO Value (putput pins)

private:
	int congigurePort();

private:
	std::string m_gpionum; // GPIO number associated with the instance of an object
	bool m_isConfigured;
};

#endif
