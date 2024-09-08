#include "Timeline.h"
#include "Collidable.h"
#include "GameEvent.h"

GameEvent::GameEvent(float timestamp, int age, EventType type, int numArgs, ...) {
	this->timestamp = timestamp;
	this->age = age;

	va_list typeArgs;
	va_start(typeArgs, numArgs);

	eventArgs = new vector<void *>();

	// Based on the event type, take in the pointer type variadic arguments using stdarg
	switch (type) {
	case CHARACTER_COLLISION:
		// Character collision takes 2 collidables that are colliding
		// Since it will be handling automovable collisions, a timeline is also needed
		eventArgs->push_back(va_arg(typeArgs, Collidable *));
		eventArgs->push_back(va_arg(typeArgs, Collidable *));
		eventArgs->push_back(va_arg(typeArgs, Timeline *));
		break;
	case CHARACTER_DEATH:
		// Character death takes a renderable to set invisible
		eventArgs->push_back(va_arg(typeArgs, Renderable *));
		break;
	case CHARACTER_SPAWN:
		// Character spawn takes a renderable and its original color
		eventArgs->push_back(va_arg(typeArgs, Renderable *));
		eventArgs->push_back(va_arg(typeArgs, Color *));
		break;
	case USER_INPUT:
		// User input takes a keycode from Keyboard::Key as an integer value, and the active character that the input affects.
		// Since user input also could imply movement and jumping, have the timeline to add velocity to
		eventArgs->push_back(va_arg(typeArgs, int *));
		eventArgs->push_back(va_arg(typeArgs, int *));
		eventArgs->push_back(va_arg(typeArgs, Timeline *));
		break;
	}

	this->type = type;
}

// This is a const function because of the requirements for comparator function
float GameEvent::getTimestamp() const {
	return this->timestamp;
}

// This is a const function because of the requirements for comparator function
int GameEvent::getAge() const {
	return this->age;
}

EventType GameEvent::getType() {
	return this->type;
}

vector<void *> *GameEvent::getEventArgs() {
	return this->eventArgs;
}