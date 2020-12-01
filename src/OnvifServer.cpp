#include "OnvifServer.h"

#include <list>
#include <algorithm>

#include <gSoap/httpda.h>
#include <gSoap/wsaapi.h>

#include <primitives/Logger.h>
#include <primitives/Config.h>

#include "Device.h"
#include "Event.h"
#include "PullPointSubscription.h"
#include "SubscriptionManager.h"
#include "SubscriptionController.h"

namespace Onvif
{

const int MAX_THREADS_COUNT = 10;

Onvif::OnvifServer& OnvifServer::getInstance()
{
	static OnvifServer server;

	return server;
}

void OnvifServer::start()
{
	LOG_INFO << "OnvifServer starting.";
	m_active = true;
	m_serverTread = std::thread([&]() {
			while (m_active)
			{
				try
				{
					if (onStartServices())
					{
						break;
					}
				}
				catch (const std::exception& e)
				{
					LOG_INFO << "Can't start services! exception: " << e.what();
				}
				
				std::this_thread::sleep_for(std::chrono::seconds(20));
			}
		});
}

void OnvifServer::stop()
{
	LOG_INFO << "OnvifServer stopping.";
	m_active = false;
	m_serverTread.join();
	LOG_INFO << "OnvifServer stopped.";
}

OnvifServer::OnvifServer()
{

}

OnvifServer::~OnvifServer()
{
	stop();
}

bool OnvifServer::onStartServices()
{
	LOG_INFO << "OnvifServer started.";

	struct soap *soap = soap_new1(SOAP_XML_STRICT | SOAP_XML_CANONICAL | SOAP_C_UTFSTRING);
	soap_set_test_logfile(soap, nullptr);
	soap_set_recv_logfile(soap, nullptr);
	soap_set_sent_logfile(soap, nullptr);
	 
	soap_register_plugin_arg(soap, http_da, http_da_md5());
	soap_register_plugin(soap, soap_wsa);
	const int port = std::stoi(Config::getInstance().getOption("port"));

	if (!soap_valid_socket(soap_bind(soap, NULL, port, 100)))
	{
		LOG_ERROR << "Can't start services! soap_bind error";
		soap_free(soap);
		return false;
	}

	soap->max_keep_alive = 0;

	std::list<std::future<void> > futureList;

	auto controller = std::make_shared<SubscriptionController>();
	
	while (m_active)
	{
		if (!soap_valid_socket(soap_accept(soap)))
		{
			LOG_ERROR << "Can't start services! soap_accept error";
			soap_free(soap);
			return false;
		}

		auto soapCopy = soap_copy(soap);

		auto device = std::make_shared<Onvif::Device>(soapCopy);
		auto event = std::make_shared<Onvif::Event>(soapCopy, controller);
		auto pullPointSubscription = std::make_shared<Onvif::PullPointSubscription>(soapCopy, controller);
		auto subscriptionManager = std::make_shared<Onvif::SubscriptionManager>(soapCopy, controller);

		while (futureList.size() >= MAX_THREADS_COUNT)
		{
			const auto& it = std::find_if(futureList.begin(), futureList.end(), 
				[](const std::future<void>& f) { return f.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready; });
			
			if (it != futureList.end())
			{
				it->wait();
				futureList.erase(it);
				continue;
			}
			futureList.front().wait_for(std::chrono::milliseconds(500));
		}

		auto f = std::async(std::launch::async, [=]()
		{
			int err = 0;

			if (soap_begin_serve(soapCopy))
			{
				LOG_ERROR << "soap_begin_serve error";
				soap_stream_fault(soapCopy, std::cerr);
			}
			else if ((err = device->dispatch()) == SOAP_NO_METHOD)
			{
				if ((err = event->dispatch()) == SOAP_NO_METHOD)
				{
					if ((err =  subscriptionManager->dispatch()) == SOAP_NO_METHOD)
					{
						err = pullPointSubscription->dispatch();
					}
				}
			}

			if (err)
			{
				soap_send_fault(soapCopy);
			}

			soap_destroy(soapCopy);
			soap_end(soapCopy);
			soap_free(soapCopy);
		});

		futureList.push_back(std::move(f));
		
	}

	LOG_INFO << "release soap pointer: soap_free .";
	soap_free(soap); // safe to delete when abc, uvw, xyz are also deleted

	return true;
}

}