#include <iostream>
#include <sysinfo.h>
#include <fstream>

#include "LicenseChecker.h"
#include "md5Util.h"


bool isKeyValid()
{
	std::stringstream sysinfo;
	
	try
	{
		sysinfo << GetMac() << "||" << GetMicroSDCID() << "||" << GetCPUSerial();
	}
	catch(std::exception& ex)
	{
		std::cerr << "Can't get system info: " << ex.what() << std::endl;
		return false;
	}
	
	auto currentSystemKey = StringToMD5(sysinfo.str());
	
	std::ifstream keyFile("/var/lib/SphinxDetectors/server.key");
	if (!keyFile.is_open())
	{
		std::cerr << "Can't open server.key file" << std::endl;
		return false;
	}
	
	std::string keyFromFile;
	keyFile >> keyFromFile;
	keyFile.close();
	
	return currentSystemKey == keyFromFile;
}
