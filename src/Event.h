#ifndef ONVIF_EVENT_H
#define ONVIF_EVENT_H

#include <gSoap/soapEventBindingService.h>

#include"SubscriptionController.h"

namespace Onvif
{

class Event : public EventBindingService
{
public:
	Event(struct soap *_soap, SubscriptionControllerSP controller);

	static tev__Capabilities* createCapabilities(struct soap* soap);

	int GetServiceCapabilities(_tev__GetServiceCapabilities *tev__GetServiceCapabilities, _tev__GetServiceCapabilitiesResponse &tev__GetServiceCapabilitiesResponse) override;
	int CreatePullPointSubscription(_tev__CreatePullPointSubscription *tev__CreatePullPointSubscription, _tev__CreatePullPointSubscriptionResponse &tev__CreatePullPointSubscriptionResponse) override;
	int GetEventProperties(_tev__GetEventProperties *tev__GetEventProperties, _tev__GetEventPropertiesResponse &tev__GetEventPropertiesResponse) override;

private:
	SubscriptionControllerSP m_controller;

	std::map<std::string, MessageType> m_filterMap;
};
}

#endif

