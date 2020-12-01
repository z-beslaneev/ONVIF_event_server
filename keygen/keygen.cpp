#include <fstream>
#include <iostream>
#include <iomanip>

#include <md5Util.h>
#include <sysinfo.h>

int main()
{	
	std::stringstream sysinfo;
	
	try
	{
		sysinfo << GetMac() << "||" << GetMicroSDCID() << "||" << GetCPUSerial();
	}
	catch(std::exception& ex)
	{
		std::cerr << "Can't get system info: " << ex.what() << std::endl;
		return -1;
	}
	
	std::ofstream keyFile("/var/lib/SphinxDetectors/server.key", std::ios_base::out | std::ios_base::trunc);
	if (!keyFile.is_open())
	{
		std::cerr << "Can't create server.key file" << std::endl;
		return -1;
	}
	
	keyFile << StringToMD5(sysinfo.str());
	keyFile.close();
	
	std::cout << "Key created succesfully!" << std::endl;

	return 0;
}
