#ifndef ONVIF_SUBSCRIPTION_CONTROLLER_H
#define ONVIF_SUBSCRIPTION_CONTROLLER_H

#include <map>
#include <queue>
#include <memory>
#include <mutex>
#include <atomic>
#include <future>
#include <ctime>

#include "EventReader.h"
#include <SoapHelpers.h>

namespace Onvif
{

extern const std::chrono::milliseconds DEFAULT_KEEP_ALIVE_TIMEOUT;

typedef std::shared_ptr<std::vector<MessageType> > FilterType;

class EventSubscription
{
public:
	EventSubscription(FilterType filter);

	std::chrono::milliseconds getTermTime();
	void setTermTime(std::chrono::milliseconds termTime);

	std::vector<NotificationMessage> getMessages(std::chrono::milliseconds waitFor, std::uint32_t maxMesages);
	void sendMessage(const NotificationMessage& message);

private:
	std::chrono::milliseconds m_termTime;

	std::condition_variable m_cv;
	std::mutex m_mutex;
	bool m_messageReceived;
	std::queue<NotificationMessage> m_messages;
	FilterType m_filter;
};

typedef std::shared_ptr<EventSubscription> EventSubscriptionSP;

class SubscriptionController 
{
public:
	SubscriptionController();
	~SubscriptionController();

	EventSubscriptionSP createSubscription(const std::string& uuid, FilterType filters);
	void removeStaleSubscriptions();
	bool removeSubscription(const std::string& uuid);

	EventSubscriptionSP getSubscription(const std::string& uuid) const;
	
	void startReadEvents();
	void stopReadEvents();


private:
	SubscriptionController(SubscriptionController const&) = delete;
	SubscriptionController& operator= (SubscriptionController const&) = delete;

	mutable std::mutex m_mutex;
	std::map<std::string, EventSubscriptionSP> m_subscriptions;

	std::atomic<bool> m_active;
	std::future<void> m_eventTreadFuture;
};

typedef std::shared_ptr<SubscriptionController> SubscriptionControllerSP;
}

#endif

