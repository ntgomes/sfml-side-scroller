#pragma once
#include <map>
#include <list>
#include "AutoMovable.h"
#include "ControllerMovable.h"
#include "Jumpable.h"
#include "Warpable.h"
#include "Gravitable.h"
#include "GameEventManager.h"
#include "ScriptManager.h"

using namespace std;

class GameObjectManager {
private:
	static GameObjectManager *instance;
	static list<PureComponent *> componentList;
	static list<Timeline *> timelines;
	static map<int, list<PureComponent *>*> objectMap;
	static bool transitioning;
	GameObjectManager();
public:
	static GameObjectManager* getInstance();

	list<PureComponent *>* getComponents();

	list<Renderable *> getRenderables();

	list<Collidable *> getCollidables();

	list<AutoMovable *> getAutoMovables();

	list<Timeline *>* getTimelines();

	map<int, list<PureComponent *>*>* getObjects();

	bool isTransitioning();

	void setTransitioning(bool b);

	void renderObjects(RenderWindow *window);

	void controlActiveCharacter(int activeCharID, Timeline *movTimer);

	void handleAutoMoves(Timeline *movTimer);

	void handleGravity(Timeline *movTimer);

	void handleCollisions(Timeline *movTimer);

	void handleWarps(RenderWindow *window);

	void deleteObject(int objectID);
};