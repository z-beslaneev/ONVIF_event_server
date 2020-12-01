#include <fstream>
#include <string>
#include <iostream>
#include <sstream>

#include <primitives/Logger.h>

#include "GPIOController.h"

GPIOClass::GPIOClass() :
m_isConfigured(false)
{
	m_gpionum = "24"; //GPIO24 is default
	congigurePort();
}

GPIOClass::GPIOClass(std::string port) :
m_isConfigured(false)
{
	m_gpionum = port;  //Instatiate GPIOClass object for GPIO pin number "gnum"
	congigurePort();
}

int GPIOClass::congigurePort()
{
	std::string export_str = "/sys/class/gpio/export";
	std::ofstream exportgpio(export_str.c_str()); // Open "export" file. Convert C++ string to C string. Required for all Linux pathnames
	if (!exportgpio.is_open())
	{
		m_isConfigured = false;
		LOG_ERROR << " OPERATION FAILED: Unable to export GPIO"<< m_gpionum;
		return -1;
	}

	exportgpio << m_gpionum ; //write GPIO number to export
	exportgpio.close(); //close export file
	
	std::string setdir_str ="/sys/class/gpio/gpio" + m_gpionum + "/direction";
	std::ofstream setdirgpio(setdir_str.c_str()); // open direction file for gpio
	if (!setdirgpio.is_open())
	{
		LOG_ERROR << " OPERATION FAILED: Unable to set direction of GPIO"<< m_gpionum;
		return -1;
	}

	setdirgpio << "out"; //write direction to direction file
	setdirgpio.close(); // close direction file
	
	m_isConfigured = true;
	
	return 0;
	
}

int GPIOClass::Write(std::string val)
{
	if(!m_isConfigured)
	{
		LOG_ERROR << " OPERATION FAILED: GPIO port is not configured" << m_gpionum;
		return -1;
	}
	
	std::string setval_str = "/sys/class/gpio/gpio" + m_gpionum + "/value";
	std::ofstream setvalgpio(setval_str.c_str()); // open value file for gpio
	if (!setvalgpio.is_open())
	{
		LOG_ERROR << " OPERATION FAILED: Unable to set the value of GPIO"<< m_gpionum;
		return -1;
	}

	setvalgpio << val ;//write value to value file
	setvalgpio.close();// close value file
	return 0;
}

GPIOClass::~GPIOClass()
{
	if(m_isConfigured)
	{
		std::string unexport_str = "/sys/class/gpio/unexport";
		std::ofstream unexportgpio(unexport_str.c_str()); //Open unexport file
		unexportgpio << m_gpionum ; //write GPIO number to unexport
		unexportgpio.close(); //close unexport file
	}
}


