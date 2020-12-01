#include "PullPointSubscription.h"

#include <chrono>
#include <thread>
#include <future>

#include <gSoap/wsaapi.h>

#include "AuthorisationHolder.h"
#include "SoapHelpers.h"
#include "sysinfo/sysinfo.h"

namespace Onvif
{

const std::string SPHINX_LOGO = "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";
const std::chrono::milliseconds MINIMAL_PULL_MESSAGE_TIMEOUT(500);

PullPointSubscription::PullPointSubscription(struct soap *_soap, SubscriptionControllerSP controller):
	PullPointSubscriptionBindingService(_soap),
	m_controller(controller)
{
}

std::string getID()
{
	static auto serial = GetCPUSerial();
	return serial;
}

wsnt__NotificationMessageHolderType* CreateMetallDetectorEvent(struct soap* soap, tt__PropertyOperation propOperation, std::chrono::milliseconds::rep cur_tssTime)
{
	auto msg = soap_new_wsnt__NotificationMessageHolderType(soap);

	msg->Topic = soap_new_wsnt__TopicExpressionTypeCustom(soap);
	msg->Topic->__any = "tns1:Device/MetalDetector/Detect";
	msg->Topic->Dialect = "http://www.onvif.org/ver10/tev/topicExpression/ConcreteSet";

	_tt__Message * tt_msg = soap_new__tt__Message(soap);

	tt_msg->UtcTime = *SoapHelpers::convertTime(soap, cur_tssTime);

	tt_msg->PropertyOperation = soap_new_tt__PropertyOperation(soap);
	*tt_msg->PropertyOperation = propOperation;

	tt_msg->Source = soap_new_tt__ItemList(soap);
	tt_msg->Source->SimpleItem.push_back(*soap_new_req__tt__ItemList_SimpleItem(soap, "Id", getID()));

	tt_msg->Key = soap_new_tt__ItemList(soap);

	tt_msg->Data = soap_new_tt__ItemList(soap);
	tt_msg->Data->SimpleItem.push_back(*soap_new_req__tt__ItemList_SimpleItem(soap, "Picture", SPHINX_LOGO));
	tt_msg->Data->SimpleItem.push_back(*soap_new_req__tt__ItemList_SimpleItem(soap, "Account", "operator"));

	msg->Message.__any.set(tt_msg, SOAP_TYPE__tt__Message);
	msg->SubscriptionReference = soap_new_wsa5__EndpointReferenceType(soap);
	msg->SubscriptionReference->Address = soap_strdup(soap, soap->endpoint);

	return msg;
}

wsnt__NotificationMessageHolderType* CreateSteamDetectorEvent(struct soap* soap, tt__PropertyOperation propOperation, std::chrono::milliseconds::rep cur_tssTime, const std::string& mesures)
{
	auto msg = soap_new_wsnt__NotificationMessageHolderType(soap);

	msg->Topic = soap_new_wsnt__TopicExpressionTypeCustom(soap);
	msg->Topic->__any = "tns1:Device/SteamDetector/Detect";
	msg->Topic->Dialect = "http://www.onvif.org/ver10/tev/topicExpression/ConcreteSet";

	_tt__Message * tt_msg = soap_new__tt__Message(soap);

	tt_msg->UtcTime = *SoapHelpers::convertTime(soap, cur_tssTime);

	tt_msg->PropertyOperation = soap_new_tt__PropertyOperation(soap);
	*tt_msg->PropertyOperation = propOperation;

	tt_msg->Source = soap_new_tt__ItemList(soap);
	tt_msg->Source->SimpleItem.push_back(*soap_new_req__tt__ItemList_SimpleItem(soap, "Id", getID()));

	tt_msg->Key = soap_new_tt__ItemList(soap);

	tt_msg->Data = soap_new_tt__ItemList(soap);
	tt_msg->Data->SimpleItem.push_back(*soap_new_req__tt__ItemList_SimpleItem(soap, "Picture", SPHINX_LOGO));
	tt_msg->Data->SimpleItem.push_back(*soap_new_req__tt__ItemList_SimpleItem(soap, "Account", "operator"));
	tt_msg->Data->SimpleItem.push_back(*soap_new_req__tt__ItemList_SimpleItem(soap, "Mesures", mesures.c_str()));

	msg->Message.__any.set(tt_msg, SOAP_TYPE__tt__Message);
	msg->SubscriptionReference = soap_new_wsa5__EndpointReferenceType(soap);
	msg->SubscriptionReference->Address = soap_strdup(soap, soap->endpoint);

	return msg;
}

wsnt__NotificationMessageHolderType* CreateRadiationMonitoringEvent(struct soap* soap, tt__PropertyOperation propOperation, std::chrono::milliseconds::rep cur_tssTime, const std::string& mesures)
{
	auto msg = soap_new_wsnt__NotificationMessageHolderType(soap);

	msg->Topic = soap_new_wsnt__TopicExpressionTypeCustom(soap);
	msg->Topic->__any = "tns1:Device/RadiationMonitoring/Detect";
	msg->Topic->Dialect = "http://www.onvif.org/ver10/tev/topicExpression/ConcreteSet";

	_tt__Message * tt_msg = soap_new__tt__Message(soap);

	tt_msg->UtcTime = *SoapHelpers::convertTime(soap, cur_tssTime);

	tt_msg->PropertyOperation = soap_new_tt__PropertyOperation(soap);
	*tt_msg->PropertyOperation = propOperation;

	tt_msg->Source = soap_new_tt__ItemList(soap);
	tt_msg->Source->SimpleItem.push_back(*soap_new_req__tt__ItemList_SimpleItem(soap, "Id", getID()));

	tt_msg->Key = soap_new_tt__ItemList(soap);

	tt_msg->Data = soap_new_tt__ItemList(soap);
	tt_msg->Data->SimpleItem.push_back(*soap_new_req__tt__ItemList_SimpleItem(soap, "Picture", SPHINX_LOGO));
	tt_msg->Data->SimpleItem.push_back(*soap_new_req__tt__ItemList_SimpleItem(soap, "Category", "Dangerous"));
	tt_msg->Data->SimpleItem.push_back(*soap_new_req__tt__ItemList_SimpleItem(soap, "Account", "operator"));
	tt_msg->Data->SimpleItem.push_back(*soap_new_req__tt__ItemList_SimpleItem(soap, "Mesures", mesures.c_str()));

	msg->Message.__any.set(tt_msg, SOAP_TYPE__tt__Message);
	msg->SubscriptionReference = soap_new_wsa5__EndpointReferenceType(soap);
	msg->SubscriptionReference->Address = soap_strdup(soap, soap->endpoint);

	return msg;
}

int PullPointSubscription::PullMessages(_tev__PullMessages *tev__PullMessages, _tev__PullMessagesResponse &tev__PullMessagesResponse)
{
	if (!AuthorisationHolder::getInstance().verifyPassword(soap))
	{
		return 401;
	}

	std::string uuid = SoapHelpers::getUuidFromHost(soap->path, "sub=");
	if (uuid.empty())
	{
		return soap_wsa_sender_fault(soap, "Invalid URI.", NULL);
	}

	auto eventSubscription = m_controller->getSubscription(uuid);

	auto cur_tssTime = SoapHelpers::getCurrentTime();

	if (!eventSubscription || eventSubscription->getTermTime().count() < cur_tssTime)
	{
		return soap_wsa_sender_fault(soap, "Invalid Subscription id", NULL);
	}

	auto waitFor =  std::chrono::duration_cast<std::chrono::milliseconds>(tev__PullMessages->Timeout);

	eventSubscription->setTermTime(std::chrono::milliseconds(cur_tssTime) + waitFor + DEFAULT_KEEP_ALIVE_TIMEOUT);

	auto messages = eventSubscription->getMessages(waitFor > MINIMAL_PULL_MESSAGE_TIMEOUT ? waitFor : MINIMAL_PULL_MESSAGE_TIMEOUT, tev__PullMessages->MessageLimit);

	cur_tssTime = SoapHelpers::getCurrentTime();

	tev__PullMessagesResponse.CurrentTime = *SoapHelpers::convertTime(this->soap, cur_tssTime);
	eventSubscription->setTermTime(std::chrono::milliseconds(cur_tssTime) + DEFAULT_KEEP_ALIVE_TIMEOUT);
	tev__PullMessagesResponse.TerminationTime = *SoapHelpers::convertTime(this->soap, eventSubscription->getTermTime().count());

	for (const auto& value : messages)
	{
		switch (value.type)
		{
		case MessageType::MetallDetector :
			tev__PullMessagesResponse.wsnt__NotificationMessage.push_back(CreateMetallDetectorEvent(soap, tt__PropertyOperation::Initialized, value.time.count()));
			break;
		case MessageType::SteamDetector:
			tev__PullMessagesResponse.wsnt__NotificationMessage.push_back(CreateSteamDetectorEvent(soap, tt__PropertyOperation::Initialized, value.time.count(), value.Mesures));
			break;
		case MessageType::RadiationMonitoring:
			tev__PullMessagesResponse.wsnt__NotificationMessage.push_back(CreateRadiationMonitoringEvent(soap, tt__PropertyOperation::Initialized, value.time.count(), value.Mesures));
			break;
		}
	}


	return soap_wsa_reply(this->soap, nullptr, "http://www.onvif.org/ver10/events/wsdl/PullPointSubscription/PullMessagesResponse");
}

int PullPointSubscription::SetSynchronizationPoint(_tev__SetSynchronizationPoint *tev__SetSynchronizationPoint, _tev__SetSynchronizationPointResponse &tev__SetSynchronizationPointResponse)
{
	if (!AuthorisationHolder::getInstance().verifyPassword(soap))
	{
		return 401;
	}

	return soap_wsa_reply(this->soap, nullptr, "http://www.onvif.org/ver10/events/wsdl/PullPointSubscription/SetSynchronizationPointResponse");
}

int PullPointSubscription::Unsubscribe(_wsnt__Unsubscribe *wsnt__Unsubscribe, _wsnt__UnsubscribeResponse &wsnt__UnsubscribeResponse)
{
	if (!AuthorisationHolder::getInstance().verifyPassword(soap))
	{
		return 401;
	}

	return soap_wsa_reply(this->soap, nullptr, "http://www.onvif.org/ver10/events/wsdl/PullPointSubscription/SetSynchronizationPointResponse");
}

}