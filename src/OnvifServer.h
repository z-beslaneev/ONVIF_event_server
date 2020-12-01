#ifndef ONVIF_SERVER_H
#define ONVIF_SERVER_H

#include <thread>
#include <future>

namespace Onvif
{

class OnvifServer 
{
public:

	static OnvifServer& getInstance();

	void start();
	void stop();
	
private:
	OnvifServer();
	~OnvifServer();
		
	OnvifServer(const OnvifServer&) = delete;
	OnvifServer& operator= (const OnvifServer&) = delete;

	bool onStartServices();
	
private:
	std::thread m_serverTread;
	std::atomic<bool> m_active;

};
}

#endif

