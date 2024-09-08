#pragma warning (disable : 4996)
#include "GameObjectManager.h"

// Defining static variables and private constructors
GameObjectManager::GameObjectManager() {};
GameObjectManager* GameObjectManager::instance;
list<PureComponent *> GameObjectManager::componentList;
list<Timeline *> GameObjectManager::timelines;
map<int, list<PureComponent *>*> GameObjectManager::objectMap;
bool GameObjectManager::transitioning = false;

// Returns the singleton instance
GameObjectManager* GameObjectManager::getInstance() {
	if (instance == nullptr) {
		instance = new GameObjectManager();
	}
	return instance;
}

// Returns the static list of pure components
list<PureComponent *>* GameObjectManager::getComponents() {
	return &componentList;
}

// Returns a list of renderables
list<Renderable *> GameObjectManager::getRenderables() {
	list<Renderable *> result;
	for (PureComponent *p : componentList) {
		if (p->getType() == ComponentType::RENDERABLE) {
			result.push_back(static_cast<Renderable *>(p));
		}
	}
	return result;
}

// Returns a list of collidables
list<Collidable *> GameObjectManager::getCollidables() {
	list<Collidable *> result;
	for (PureComponent *p : componentList) {
		if (p->getType() == ComponentType::COLLIDABLE) {
			result.push_back(static_cast<Collidable *>(p));
		}
	}
	return result;
}

// Returns a list of automovables
list<AutoMovable *> GameObjectManager::getAutoMovables() {
	list<AutoMovable *> result;
	for (PureComponent *p : componentList) {
		if (p->getType() == ComponentType::AUTOMOVABLE) {
			result.push_back(static_cast<AutoMovable *>(p));
		}
	}
	return result;
}

list<Timeline *>* GameObjectManager::getTimelines() {
	return &timelines;
}

// Returns the static map of object ID keys and the pure component list linked
// to that object ID.
map<int, list<PureComponent *>*>* GameObjectManager::getObjects() {
	return &objectMap;
}

// Getter for transitioning member bool
bool GameObjectManager::isTransitioning() {
	return this->transitioning;
}

// Setter for transitioning member bool
void GameObjectManager::setTransitioning(bool b) {
	this->transitioning = b;
}

// Function that handles rendering all renderables.
// Will iterate through each object's list of components and render its renderable component.
void GameObjectManager::renderObjects(RenderWindow *window) {
	window->clear();
	for (auto it = objectMap.begin(); it != objectMap.end(); ++it) {
		list<PureComponent *> *compList = it->second;
		for (PureComponent *p : *compList) {
			if (p->getType() == ComponentType::RENDERABLE) {
				window->draw(*(static_cast<Renderable *>(p)->getRender()));
				break;
			}
		}
	}
	window->display();
}

// Checks for ControllerMovable input whose object has an id equal to activeCharID.
// It has this parameter for server-client logic.
void GameObjectManager::controlActiveCharacter(int activeCharID, Timeline *movTimer) {
	if (!transitioning) {
		for (PureComponent *p : componentList) {
			if (Keyboard::isKeyPressed(Keyboard::Key::A)) {
				int *aCode = new int(Keyboard::Key::A);
				int *charToMove = new int(activeCharID);
				GameEvent *aPressedEvent = new GameEvent(GameEventManager::getInstance()->getCurrentEventTime(), 0, EventType::USER_INPUT, 3, aCode, charToMove, movTimer);
				GameEventManager::getInstance()->raiseEvent(aPressedEvent);
			}
			if (Keyboard::isKeyPressed(Keyboard::Key::D)) {
				int *dCode = new int(Keyboard::Key::D);
				int *charToMove = new int(activeCharID);
				GameEvent *dPressedEvent = new GameEvent(GameEventManager::getInstance()->getCurrentEventTime(), 0, EventType::USER_INPUT, 3, dCode, charToMove, movTimer);
				GameEventManager::getInstance()->raiseEvent(dPressedEvent);
			}
			if (p->getType() == ComponentType::JUMPABLE && p->getObjectId() == activeCharID) {
				Jumpable *j = static_cast<Jumpable *>(p);
				if (Keyboard::isKeyPressed(Keyboard::Key::W)) {
					int *wCode = new int(Keyboard::Key::W);
					int *charToJump = new int(activeCharID);
					GameEvent *wPressedEvent = new GameEvent(GameEventManager::getInstance()->getCurrentEventTime(), 0, EventType::USER_INPUT, 3, wCode, charToJump, movTimer);
					GameEventManager::getInstance()->raiseEvent(wPressedEvent);
				}
				// Then run the jumpable's jump
				j->jump(movTimer);
			}
		}
	}
}

// Move all objects with automovables with non-zero speed
void GameObjectManager::handleAutoMoves(Timeline *movTimer) {
	for (PureComponent *p : componentList) {
		if (p->getType() == ComponentType::AUTOMOVABLE) {
			// Automovable component movement will be handled by scripting
			AutoMovable *am = static_cast<AutoMovable *>(p);
			char am_ca[50];
			char mt_ca[50];
			sprintf(am_ca, "0x%p", am);
			sprintf(mt_ca, "0x%p", movTimer);
			string am_s(am_ca);
			string mt_s(mt_ca);

			// The actual JavaScript, which sets the proper variables and calls a native function
			string *amJS = new string("var am = " + am_s + ";" +
									  "var mt = " + mt_s + ";" +
									  "moveAutoMovable(am, mt);");

			// Execute the JavaScript
			ScriptManager::getInstance()->runScript(amJS);
		}
	}
}

// Cause all objects with gravitables to be affected by gravity
void GameObjectManager::handleGravity(Timeline *movTimer) {
	for (PureComponent *p : componentList) {
		if (p->getType() == ComponentType::GRAVITABLE) {
			(static_cast<Gravitable *>(p))->fall(movTimer);
		}
	}
}

// Function for handling collisions
void GameObjectManager::handleCollisions(Timeline *movTimer) {
	for (PureComponent *p : componentList) {
		// First, navigate to the given collidable
		if (p->getType() == ComponentType::COLLIDABLE) {
			Collidable *col = (static_cast<Collidable *>(p));
			Collidable *contactCol = nullptr;
			int totalCollisions = 0;
			int contactShapeId = -1;
			// Then, check to see if any other collidable is colliding with that given collidable
			for (PureComponent *p2 : componentList) {
				if (p2->getType() == ComponentType::COLLIDABLE && p2 != col) {
					if (col->collidesWith((static_cast<Collidable *>(p2)))) {
						contactCol = static_cast<Collidable *>(p2);
						contactShapeId = (static_cast<Collidable *>(p2))->getObjectId();
						totalCollisions++;
					}
				}
			}
			if (totalCollisions) {
				GameEvent *collisionHandlingEvent = new GameEvent(GameEventManager::getInstance()->getCurrentEventTime(), 0, EventType::CHARACTER_COLLISION, 3, col, contactCol, movTimer);
				GameEventManager::getInstance()->raiseEvent(collisionHandlingEvent);
			}
			else {
				col->setContactShape(nullptr);
			}
		}
	}
}

// Function for handling death zones and any boundary essentially
void GameObjectManager::handleWarps(RenderWindow *window) {
	for (PureComponent *p : componentList) {
		if (p->getType() == ComponentType::WARPABLE) {
			Warpable *warpable = (static_cast<Warpable *>(p));
			switch (warpable->getType()) {
			case WarpType::DEATH_ZONE:
				warpable->killCharacter();
				break;
			case WarpType::LEFT_BOUNDARY:
				warpable->leftBoundTrigger(window);
				break;
			case WarpType::RIGHT_BOUNDARY:
				warpable->rightBoundTrigger(window);
				break;
			}
		}
	}
}

// This function is called whenever a client disconnects. Erases all components relating to that client's player
void GameObjectManager::deleteObject(int objectID) {
	for (PureComponent *p : *objectMap.at(objectID)) {
		componentList.remove(p);
	}
	objectMap.erase(objectID);
}