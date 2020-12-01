#ifndef EVENT_READER
#define EVENT_READER

#include <string>
#include <vector>
#include <algorithm>
#include <chrono>
#include <list>

#include "SoapHelpers.h"

#ifndef WIN32
#include "SerialController.h"
#endif

class EventReader
{
	EventReader(const EventReader&) = delete;
	EventReader& operator=(EventReader) = delete;

	public:

	EventReader();
	~EventReader();

	std::vector<NotificationMessage> ReadEvents();

	private:
	std::list<std::uint8_t> m_buffer;
#ifndef WIN32
	SerialControllerP m_controller;
#endif
};

#endif
