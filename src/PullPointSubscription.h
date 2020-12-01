#ifndef ONVIF_PULLPOINT_SUBSCRIPTION_H
#define ONVIF_PULLPOINT_SUBSCRIPTION_H

#include <gSoap/soapPullPointSubscriptionBindingService.h>
#include <SubscriptionController.h>

namespace Onvif
{

class PullPointSubscription : public PullPointSubscriptionBindingService
{
public:
	PullPointSubscription(struct soap *_soap, SubscriptionControllerSP controller);

	int PullMessages(_tev__PullMessages *tev__PullMessages, _tev__PullMessagesResponse &tev__PullMessagesResponse) override;
	int SetSynchronizationPoint(_tev__SetSynchronizationPoint *tev__SetSynchronizationPoint, _tev__SetSynchronizationPointResponse &tev__SetSynchronizationPointResponse) override;
	int Unsubscribe(_wsnt__Unsubscribe *wsnt__Unsubscribe, _wsnt__UnsubscribeResponse &wsnt__UnsubscribeResponse) override;

private:
	SubscriptionControllerSP m_controller;
};
}

#endif

