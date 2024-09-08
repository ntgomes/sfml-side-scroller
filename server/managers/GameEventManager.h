#pragma once
#include <vector>
#include <queue>
#include "GameObjectManager.h"
#include "ReplayManager.h"
#include "ScriptManager.h"
#include "GameEvent.h"

using namespace std;

class GameEventManager {
private:
	class GameEventComparator {
	public:
		GameEventComparator();
		bool operator() (const GameEvent *lhs, const GameEvent *rhs) const;
	};

	static GameEventManager *instance;
	static priority_queue<GameEvent *, vector<GameEvent *>, GameEventComparator> eventQueue;
	static Timeline *eventTime;
	static Mutex eventQLock;
	GameEventManager();
public:
	static GameEventManager* getInstance();
	void raiseEvent(GameEvent *e);
	void handleEvents();
	float getCurrentEventTime();
};