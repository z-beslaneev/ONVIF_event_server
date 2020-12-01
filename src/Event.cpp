#include "Event.h"

#include <gSoap/wsaapi.h>

#include "AuthorisationHolder.h"
#include "SoapHelpers.h"


namespace Onvif
{
soap_dom_element createMetalDetectorDescription(struct soap* soap);
soap_dom_element createSteamDetectorDescription(struct soap* soap);
soap_dom_element createRadiationMonitoringDescription(struct soap* soap);

Event::Event(struct soap *_soap, SubscriptionControllerSP controller):
	EventBindingService(_soap),
	m_controller(controller)
{
	m_filterMap = 
	{
		{"tns1:Device/MetalDetector/Detect",		MessageType::MetallDetector},
		{"tns1:Device/tmk:MetalDetector/Detect",		MessageType::MetallDetector},
		{"tns1:Device/tmk:MetalDetector/tmk:Detect",		MessageType::MetallDetector},
		{"tns1:Device/SteamDetector/Detect",		MessageType::SteamDetector},
		{"tns1:Device/tmk:SteamDetector/Detect",		MessageType::SteamDetector},
		{"tns1:Device/tmk:SteamDetector/tmk:Detect",		MessageType::SteamDetector},
		{"tns1:Device/RadiationMonitoring/Detect",	MessageType::RadiationMonitoring},
		{"tns1:Device/tmk:RadiationMonitoring/Detect",	MessageType::RadiationMonitoring},
		{"tns1:Device/tmk:RadiationMonitoring/tmk:Detect",	MessageType::RadiationMonitoring}
	};
}

tev__Capabilities* Event::createCapabilities(struct soap* soap)
{
	auto eventCapabilities = soap_new_req_tev__Capabilities(soap);

	using namespace SoapHelpers;
	eventCapabilities->WSSubscriptionPolicySupport = soap_new_req_bool(soap, false);
	eventCapabilities->WSPullPointSupport = soap_new_req_bool(soap, true);
	eventCapabilities->WSPausableSubscriptionManagerInterfaceSupport = soap_new_req_bool(soap, false);
	eventCapabilities->MaxPullPoints = soap_new_req_int(soap, 8);

	return eventCapabilities;
}

int Event::GetServiceCapabilities(_tev__GetServiceCapabilities *tev__GetServiceCapabilities, _tev__GetServiceCapabilitiesResponse &tev__GetServiceCapabilitiesResponse)
{
	if (!AuthorisationHolder::getInstance().verifyPassword(soap))
	{
		return 401;
	}

	tev__GetServiceCapabilitiesResponse.Capabilities = createCapabilities(soap);
	return SOAP_OK;
}

int Event::CreatePullPointSubscription(_tev__CreatePullPointSubscription *tev__CreatePullPointSubscription, _tev__CreatePullPointSubscriptionResponse &tev__CreatePullPointSubscriptionResponse)
{
	if (!AuthorisationHolder::getInstance().verifyPassword(soap))
	{
		return 401;
	}
	FilterType filter;
	if (tev__CreatePullPointSubscription->Filter)
	{
		filter.reset(new FilterType::element_type());
		for (auto value : tev__CreatePullPointSubscription->Filter->__any)
		{
			auto it = m_filterMap.find(value.text);

			if (it == m_filterMap.end())
			{
				return soap_wsa_sender_fault(soap, "Invalid Filter", NULL);
			}
			filter->push_back(it->second);
		}
		
	}
	std::string uuid = soap_rand_uuid(soap, "");

	auto eventSubscription = m_controller->createSubscription(uuid, filter);

	std::string str = SoapHelpers::getHost(soap, std::string("?sub=") + uuid);
	tev__CreatePullPointSubscriptionResponse.SubscriptionReference.Address = soap_strdup(soap, str.c_str());

	auto currTime = SoapHelpers::getCurrentTime();
	tev__CreatePullPointSubscriptionResponse.wsnt__CurrentTime = *SoapHelpers::convertTime(soap, currTime);

	auto lifetime = std::chrono::duration_cast<std::chrono::nanoseconds>(DEFAULT_KEEP_ALIVE_TIMEOUT);

	if (tev__CreatePullPointSubscription->InitialTerminationTime)
	{
		soap_s2xsd__duration(this->soap, tev__CreatePullPointSubscription->InitialTerminationTime->c_str(), &lifetime);
	}
	auto termTime = std::chrono::milliseconds(currTime + std::chrono::duration_cast<std::chrono::milliseconds>(lifetime).count());
	eventSubscription->setTermTime(termTime);

	tev__CreatePullPointSubscriptionResponse.wsnt__TerminationTime = *SoapHelpers::convertTime(
		soap, eventSubscription->getTermTime().count());
	
	return soap_wsa_reply(this->soap, nullptr, "http://www.onvif.org/ver10/events/wsdl/EventPortType/CreatePullPointSubscriptionResponse");
}

static soap_dom_element AddHolder(struct soap* soap, soap_dom_element& child, const char* holderName, bool isTopic)
{
	auto holder = soap_new_tt__AnyHolder(soap);
	holder->__any.push_back(child);

	if (isTopic)
	{
		holder->__anyAttribute.name = "wstop:topic";
		holder->__anyAttribute.text = soap_bool2s(soap, true);
	}

	soap_dom_element holderEl(soap);
	holderEl.set(holder, SOAP_TYPE_tt__AnyHolder);
	holderEl.name = holderName;

	return holderEl;
}

int Event::GetEventProperties(_tev__GetEventProperties *tev__GetEventProperties, _tev__GetEventPropertiesResponse &tev__GetEventPropertiesResponse)
{
	if (!AuthorisationHolder::getInstance().verifyPassword(soap))
	{
		return 401;
	}

	tev__GetEventPropertiesResponse.TopicNamespaceLocation.push_back("http://www.onvif.org/onvif/ver10/topics/topicns.xml");
	tev__GetEventPropertiesResponse.wsnt__FixedTopicSet = true;
	tev__GetEventPropertiesResponse.wsnt__TopicExpressionDialect.push_back("http://www.onvif.org/ver10/tev/topicExpression/ConcreteSet");
	tev__GetEventPropertiesResponse.wsnt__TopicExpressionDialect.push_back("http://docs.oasis-open.org/wsn/t-1/TopicExpression/Concrete");
	tev__GetEventPropertiesResponse.MessageContentFilterDialect.push_back("http://www.onvif.org/ver10/tev/messageContentFilter/ItemFilter");
	tev__GetEventPropertiesResponse.MessageContentSchemaLocation.push_back("http://www.onvif.org/onvif/ver10/schema/onvif.xsd");

	tev__GetEventPropertiesResponse.wstop__TopicSet = tev__GetEventPropertiesResponse.wstop__TopicSet = soap_new_wstop__TopicSetType(soap);

	auto holder = soap_new_tt__AnyHolder(soap);
	holder->__any.push_back(createMetalDetectorDescription(soap));
	holder->__any.push_back(createSteamDetectorDescription(soap));
	holder->__any.push_back(createRadiationMonitoringDescription(soap));

	soap_dom_element DeviceEl(soap);
	DeviceEl.set(holder, SOAP_TYPE_tt__AnyHolder);
	DeviceEl.name = "tns1:Device";

	tev__GetEventPropertiesResponse.wstop__TopicSet->__any.push_back(DeviceEl);

	return soap_wsa_reply(this->soap, nullptr, "http://www.onvif.org/ver10/events/wsdl/EventPortType/GetEventPropertiesResponse");
}

soap_dom_element createMetalDetectorDescription(struct soap* soap)
{
	tt__MessageDescription * detectMessage = soap_new_tt__MessageDescription(soap);

	detectMessage->Source = soap_new_tt__ItemListDescription(soap);

	_tt__ItemListDescription_SimpleItemDescription sourceDescr;
	sourceDescr.Name = "Id";
	sourceDescr.Type = "xsd:string";
	detectMessage->Source->SimpleItemDescription.push_back(sourceDescr);

	detectMessage->Data = soap_new_tt__ItemListDescription(soap);

	_tt__ItemListDescription_SimpleItemDescription PictureDescr;
	PictureDescr.Name = "Picture";
	PictureDescr.Type = "xsd:string";
	detectMessage->Data->SimpleItemDescription.push_back(PictureDescr);

	_tt__ItemListDescription_SimpleItemDescription elemDescr;
	elemDescr.Name = "Account";
	elemDescr.Type = "xsd:string";
	detectMessage->Data->SimpleItemDescription.push_back(elemDescr);

	soap_dom_element msgDescr(soap);
	msgDescr.set(detectMessage, SOAP_TYPE_tt__MessageDescription);
	msgDescr.name = "tt:MessageDescription";

	soap_dom_element DetectEl = AddHolder(soap, msgDescr, "tmk:Detect", true);
	soap_dom_element MetalDetectorEl = AddHolder(soap, DetectEl, "tmk:MetalDetector", false);

	return MetalDetectorEl;
}

soap_dom_element createSteamDetectorDescription(struct soap* soap)
{
	tt__MessageDescription * detectMessage = soap_new_tt__MessageDescription(soap);

	detectMessage->Source = soap_new_tt__ItemListDescription(soap);

	_tt__ItemListDescription_SimpleItemDescription sourceDescr;
	sourceDescr.Name = "Id";
	sourceDescr.Type = "xsd:string";
	detectMessage->Source->SimpleItemDescription.push_back(sourceDescr);

	detectMessage->Data = soap_new_tt__ItemListDescription(soap);

	_tt__ItemListDescription_SimpleItemDescription PictureDescr;
	PictureDescr.Name = "Picture";
	PictureDescr.Type = "xsd:string";
	detectMessage->Data->SimpleItemDescription.push_back(PictureDescr);

	_tt__ItemListDescription_SimpleItemDescription elemDescr;
	elemDescr.Name = "Account";
	elemDescr.Type = "xsd:string";
	detectMessage->Data->SimpleItemDescription.push_back(elemDescr);

	soap_dom_element msgDescr(soap);
	msgDescr.set(detectMessage, SOAP_TYPE_tt__MessageDescription);
	msgDescr.name = "tt:MessageDescription";

	soap_dom_element DetectEl = AddHolder(soap, msgDescr, "tmk:Detect", true);
	soap_dom_element SteamDetectorEl = AddHolder(soap, DetectEl, "tmk:SteamDetector", false);

	return SteamDetectorEl;
}

soap_dom_element createRadiationMonitoringDescription(struct soap* soap)
{
	tt__MessageDescription * detectMessage = soap_new_tt__MessageDescription(soap);

	detectMessage->Source = soap_new_tt__ItemListDescription(soap);

	_tt__ItemListDescription_SimpleItemDescription sourceDescr;
	sourceDescr.Name = "Id";
	sourceDescr.Type = "xsd:string";
	detectMessage->Source->SimpleItemDescription.push_back(sourceDescr);

	detectMessage->Data = soap_new_tt__ItemListDescription(soap);

	_tt__ItemListDescription_SimpleItemDescription PictureDescr;
	PictureDescr.Name = "Picture";
	PictureDescr.Type = "xsd:string";
	detectMessage->Data->SimpleItemDescription.push_back(PictureDescr);

	_tt__ItemListDescription_SimpleItemDescription elemDescr;
	elemDescr.Name = "Account";
	elemDescr.Type = "xsd:string";
	detectMessage->Data->SimpleItemDescription.push_back(elemDescr);

	soap_dom_element msgDescr(soap);
	msgDescr.set(detectMessage, SOAP_TYPE_tt__MessageDescription);
	msgDescr.name = "tt:MessageDescription";

	soap_dom_element DetectEl = AddHolder(soap, msgDescr, "tmk:Detect", true);
	soap_dom_element RadiationMonitoringEl = AddHolder(soap, DetectEl, "tmk:RadiationMonitoring", false);

	return RadiationMonitoringEl;
}

}