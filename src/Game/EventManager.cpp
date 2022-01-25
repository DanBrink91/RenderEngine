#include "EventManager.h"

uint64_t EventManager::subscribe(EventType type, EventCallback func)
{
	Subscriber s = Subscriber(_eventSubscribers[type].nextID, func);
	_eventSubscribers[type].subscribers.push_back(s);
	return _eventSubscribers[type].nextID++;
}

void EventManager::unsubscribe(EventType type, uint64_t id) 
{
	auto findType = _eventSubscribers.find(type);
	if (findType != _eventSubscribers.end())
	{
		for(int i = 0; i < _eventSubscribers[type].subscribers.size(); i++)
		{
			auto& handler = _eventSubscribers[type].subscribers[i];
			if(handler.id == id)
			{
				_eventSubscribers[type].subscribers.erase(_eventSubscribers[type].subscribers.begin() + i);
				break;
			}
		}
	}
}

void EventManager::notify(EventType type, Event* event) 
{
	auto find = _eventSubscribers.find(type);
	if (find != _eventSubscribers.end())
	{
		for(auto& subscriber: _eventSubscribers[type].subscribers)
			subscriber.callback(event);
	}
}