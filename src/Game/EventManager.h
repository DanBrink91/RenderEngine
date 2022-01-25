#pragma once
#include <functional>
#include <vector>
#include <unordered_map>

enum EventType {
	EventTypeBattleStart = 0,
	EventTypeBattleEndVictory,
	EventTypeTurnStart,
	EventTypeTurnEnd,
	EventTypeAbilityUsed,
	EventTypeDamageDone,
	EventTypeDamageBlocked
};

struct Event {
	EventType type;
};

struct BattleStartEvent : Event {
	int enemyType;
};


typedef std::function<void (struct Event*)> EventCallback;

struct Subscriber
{
	uint64_t id;
	EventCallback callback;

	Subscriber(uint64_t ID, EventCallback Callback)
	:id(ID), callback(Callback)
	{};
	// copy
	Subscriber(const Subscriber& other)
	: id(other.id), callback(other.callback)
	{};
	// move
	Subscriber(Subscriber&& other)
	: id(other.id), callback(other.callback)
	{};

	Subscriber& operator=(Subscriber&& other)
	{
		id = other.id;
		callback = other.callback;
		return *this;
	};
};

struct EventHandler
{
	uint64_t nextID = 0;
	std::vector<Subscriber> subscribers;
};


class EventManager {
public:
	uint64_t subscribe(EventType type, EventCallback func);
	void unsubscribe(EventType type, uint64_t id);
	void notify(EventType type, Event* event);

private:
	std::unordered_map<EventType, EventHandler> _eventSubscribers;
	
};