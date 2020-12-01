#include "SubscriptionManager.h"

#include <gSoap/wsaapi.h>

#include "AuthorisationHolder.h"
#include "SoapHelpers.h"

namespace Onvif
{

SubscriptionManager::SubscriptionManager(struct soap *_soap, SubscriptionControllerSP controller):
	SubscriptionManagerBindingService(_soap),
	m_controller(controller)
{
}


int SubscriptionManager::Renew(_wsnt__Renew *wsnt__Renew, _wsnt__RenewResponse &wsnt__RenewResponse)
{
	if (!AuthorisationHolder::getInstance().verifyPassword(soap))
	{
		return 401;
	}
	std::string uuid = SoapHelpers::getUuidFromHost(soap->path, "sub=");
	if (uuid.empty())
	{
		return soap_wsa_sender_fault(soap, "Invalid URI", NULL);
	}

	auto eventSubscription = m_controller->getSubscription(uuid);

	auto currTime = SoapHelpers::getCurrentTime();

	if (!eventSubscription || eventSubscription->getTermTime().count() < currTime)
	{
		return soap_wsa_sender_fault(soap, "Invalid Subscription id", NULL);
	}

	auto lifetime = std::chrono::duration_cast<std::chrono::nanoseconds>(DEFAULT_KEEP_ALIVE_TIMEOUT);
	if (wsnt__Renew->TerminationTime)
	{
		soap_s2xsd__duration(this->soap, wsnt__Renew->TerminationTime->c_str(), &lifetime);
	}

	auto termTime = std::chrono::milliseconds(currTime + std::chrono::duration_cast<std::chrono::milliseconds>(lifetime).count());
	eventSubscription->setTermTime(termTime);
	
	wsnt__RenewResponse.CurrentTime = SoapHelpers::convertTime(soap, currTime);
	wsnt__RenewResponse.TerminationTime = *SoapHelpers::convertTime(soap, termTime.count());

	return soap_wsa_reply(this->soap, nullptr, "http://docs.oasis-open.org/wsn/bw-2/SubscriptionManager/RenewResponse");
}

int SubscriptionManager::Unsubscribe(_wsnt__Unsubscribe *wsnt__Unsubscribe, _wsnt__UnsubscribeResponse &wsnt__UnsubscribeResponse)
{
	if (!AuthorisationHolder::getInstance().verifyPassword(soap))
	{
		return 401;
	}

	std::string uuid = SoapHelpers::getUuidFromHost(soap->path, "sub=");

	if (uuid.empty() || !m_controller->removeSubscription(uuid))
	{
		return soap_wsa_sender_fault(soap, "Invalid Subscription id", NULL);
	}

	return soap_wsa_reply(this->soap, nullptr, "http://docs.oasis-open.org/wsn/bw-2/SubscriptionManager/UnsubscribeResponse");
}

}