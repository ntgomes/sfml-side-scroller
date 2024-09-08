#pragma once
#include <vector>
#include <stdarg.h>

using namespace std;

/// The different event types for GameEvent.
/// Registering a new event means adding it to this enum.
enum EventType {
	CHARACTER_COLLISION,
	CHARACTER_DEATH,
	CHARACTER_SPAWN,
	USER_INPUT,
	START_RECORDING,
	STOP_RECORDING
};

class GameEvent {
private:
	float timestamp;				// The mandatory timestamp field (represented as floating point)
	int age;						// The age field (used only to distinguish chaining of events)
	EventType type;					// The EventType enum used to distinguish event type
	vector<void *> *eventArgs;		// The vector of void pointers denoting event type parameters
public:
	GameEvent(float timestamp, int age, EventType type, int numArgs, ...);

	// Getters for each field (which allows for Serializable events)
	float getTimestamp() const;
	int getAge() const;
	EventType getType();
	vector<void *>* getEventArgs();
};