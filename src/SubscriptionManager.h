#ifndef ONVIF_SUBSCRIPTION_MANAGER_H
#define ONVIF_SUBSCRIPTION_MANAGER_H

#include <gSoap/soapSubscriptionManagerBindingService.h>
#include<SubscriptionController.h>

namespace Onvif
{

class SubscriptionManager : public SubscriptionManagerBindingService
{
public:
	SubscriptionManager(struct soap *_soap, SubscriptionControllerSP controller);

	int Renew(_wsnt__Renew *wsnt__Renew, _wsnt__RenewResponse &wsnt__RenewResponse) override;
	int Unsubscribe(_wsnt__Unsubscribe *wsnt__Unsubscribe, _wsnt__UnsubscribeResponse &wsnt__UnsubscribeResponse) override;

private:
	SubscriptionControllerSP m_controller;
};
}

#endif

