#pragma warning (disable : 4996)
#include "GameEventManager.h"

// Defining static variables and private constructors
GameEventManager::GameEventManager() {};
GameEventManager* GameEventManager::instance;
Timeline* GameEventManager::eventTime;
Mutex GameEventManager::eventQLock;			// We need this mutex because threads will be adding and handling events at the same time; C++ doesn't like that
priority_queue<GameEvent *, vector<GameEvent *>, GameEventManager::GameEventComparator> GameEventManager::eventQueue(GameEventManager::GameEventComparator::GameEventComparator());

// Define inner comparator class functions
GameEventManager::GameEventComparator::GameEventComparator() {};
bool GameEventManager::GameEventComparator::operator() (const GameEvent *lhs, const GameEvent *rhs) const {
	return lhs->getTimestamp() < rhs->getTimestamp();
}

// Returns the singleton instance
GameEventManager* GameEventManager::getInstance() {
	if (instance == nullptr) {
		instance = new GameEventManager();
		eventTime = new Timeline();
	}
	return instance;
}

void GameEventManager::raiseEvent(GameEvent *e) {
	eventQLock.lock();
	eventQueue.push(e);
	eventQLock.unlock();
}

float GameEventManager::getCurrentEventTime() {
	return eventTime->getTime();
}

void GameEventManager::handleEvents() {
	eventQLock.lock();
	while (!eventQueue.empty()) {
		GameEvent *current = eventQueue.top();
		eventQueue.pop();

		// Handle the event based on its type
		switch (current->getType()) {
		case EventType::CHARACTER_COLLISION:
		{
			Collidable *first = static_cast<Collidable *>(current->getEventArgs()->at(0));
			Collidable *second = static_cast<Collidable *>(current->getEventArgs()->at(1));
			Timeline *movTimer = static_cast<Timeline *>(current->getEventArgs()->at(2));

			// The event handling for collisions here is making sure that the collidables are keeping their
			// speeds relative to each other
			try {
				AutoMovable *pa = nullptr;
				map<int, list<PureComponent *>*> *objectMap = GameObjectManager::getInstance()->getObjects();
				for (PureComponent *p3 : *objectMap->at(first->getObjectId())) {
					if (p3->getType() == ComponentType::AUTOMOVABLE) {
						pa = (static_cast<AutoMovable *>(p3));
					}
				}
				// If something is touching the automovable and the contact's automovable is not null
				if (pa != nullptr) {
					for (PureComponent *p4 : *objectMap->at(second->getObjectId())) {
						if (p4->getType() == ComponentType::RENDERABLE && static_cast<Renderable *>(p4)->getRenderType() == RenderType::CHARACTER) {
							float movTimeQuanta = movTimer->getTime();
							static_cast<Renderable *>(p4)->getRender()->move(pa->getXSpeed() * movTimeQuanta, pa->getYSpeed() * movTimeQuanta);
						}
					}
				}
			}
			catch (out_of_range e) {
				// We catch this error in case an object is deleted while the thread that handles this is running
			}
		}
		break;
		case EventType::CHARACTER_DEATH:
		{
			// Character death event will be handled by scripting logic
			Renderable *charToKill = static_cast<Renderable *>(current->getEventArgs()->at(0));

			char kc_ca[50];
			sprintf(kc_ca, "0x%p", charToKill);

			string kc_s(kc_ca);

			// The resultant JavaScript
			string *deathJS = new string("var charKill = " + kc_s +";" +
										 "const invisibleColor = 0;" +
										 "killChar(charKill, invisibleColor);");

			// Execute the JavaScript
			ScriptManager::getInstance()->runScript(deathJS);
		}
		break;
		case EventType::CHARACTER_SPAWN:
		{
			// Get the renderable from the args
			Renderable *charToSpawn = static_cast<Renderable *>(current->getEventArgs()->at(0));
			// Get the color from the args
			Color *originalColor = static_cast<Color *>(current->getEventArgs()->at(1));

			// Set the renderable's color back to its original
			charToSpawn->setColor(*originalColor);
			// Finally, spawn the character by setting its position to the spawn point
			charToSpawn->setPosition(Vector2f(300.f, 400.f));
		}
		break;
		case EventType::USER_INPUT:
		{
			// Get the keycode from the args
			int *intKeyCode = static_cast<int *>(current->getEventArgs()->at(0));
			// Get the affected character's ID from the args
			int *affectedCharID = static_cast<int *>(current->getEventArgs()->at(1));
			// Get the move timeline from the args
			Timeline *movTimer = static_cast<Timeline *>(current->getEventArgs()->at(2));

			switch (*intKeyCode) {
			case Keyboard::Key::A:
			{
				// Now get the affected character as the ControllerMovable
				ControllerMovable *character = nullptr;
				for (PureComponent *p : *GameObjectManager::getInstance()->getComponents()) {
					if (p->getType() == ComponentType::CONTROLLERMOVABLE && p->getObjectId() == *affectedCharID) {
						character = (static_cast<ControllerMovable *>(p));
					}
				}
				character->move(MoveDirection::LEFT, movTimer);
			}
			break;
			case Keyboard::Key::D:
			{
				// Now get the affected character as the ControllerMovable
				ControllerMovable *character = nullptr;
				for (PureComponent *p : *GameObjectManager::getInstance()->getComponents()) {
					if (p->getType() == ComponentType::CONTROLLERMOVABLE && p->getObjectId() == *affectedCharID) {
						character = (static_cast<ControllerMovable *>(p));
					}
				}
				character->move(MoveDirection::RIGHT, movTimer);
			}
			break;
			case Keyboard::Key::W:
			{
				Jumpable *jumper = nullptr;
				for (PureComponent *p : *GameObjectManager::getInstance()->getComponents()) {
					if (p->getType() == ComponentType::JUMPABLE && p->getObjectId() == *affectedCharID) {
						jumper = (static_cast<Jumpable *>(p));
						Collidable *col = nullptr;
						// Every jumpable is assumed to be collideable, so get the collidable
						for (PureComponent *p2 : *GameObjectManager::getInstance()->getComponents()) {
							if (p2->getType() == ComponentType::COLLIDABLE && p2->getObjectId() == p->getObjectId()) {
								col = (static_cast<Collidable *>(p2));
								break;
							}
						}
						if (!jumper->isJumping() && col->touching()) {
							jumper->prepareToJump();
							col->setContactShape(nullptr);
						}
					}
				}
			}
			break;
			case Keyboard::Key::N:
			{
				// The N key is for starting a new recording, which delegates a new recording event
				GameEvent *startRecordingEvent = new GameEvent(getCurrentEventTime(), current->getAge() + 1, EventType::START_RECORDING, 0);
				raiseEvent(startRecordingEvent);
			}
			break;
			case Keyboard::Key::S:
			{
				// The S key is for stopping a new recording, which delegates a new recording event
				GameEvent *stopRecordingEvent = new GameEvent(getCurrentEventTime(), current->getAge() + 1, EventType::STOP_RECORDING, 0);
				raiseEvent(stopRecordingEvent);
			}
			break;
			}
		}
		break;
		case EventType::START_RECORDING:
			ReplayManager::getInstance()->startRecording();
			break;
		case EventType::STOP_RECORDING:
			ReplayManager::getInstance()->stopRecording();
			break;
		}
	}
	eventQLock.unlock();
}