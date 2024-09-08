#include "GameObjectFactory.h"

GameObjectFactory::GameObjectFactory() {};
GameObjectFactory* GameObjectFactory::instance;

// Returns singleton instance
GameObjectFactory* GameObjectFactory::getInstance() {
	if (instance = nullptr) {
		instance = new GameObjectFactory();
	}
	return instance;
}

// Generates a random 32-bit integer for object ID
int GameObjectFactory::generateRandomObjectID() {
	srand(time(NULL));
	int result = rand();

	// Checks if the result is already a key in the manager's object map
	while (true) {
		try {
			GameObjectManager::getInstance()->getObjects()->at(result);
			result = rand();
		}
		catch (out_of_range e) {
			break;
		}
	}

	return result;
}

// Creates a new randomly character with the given dimensions
int GameObjectFactory::createCharacter(Vector2f dimensions) {
	// A character is Renderable, Collidable, ControllerMovable, Gravitable, and Jumpable
	int objectId = generateRandomObjectID();

	Renderable *renderable = new Renderable(rand(), objectId, RenderType::CHARACTER, vector<Vector2f>({ dimensions }));
	//renderable->setPosition(Vector2f(300.f, 400.f));
	Collidable *collidable = new Collidable(rand(), objectId, renderable);

	ControllerMovable *controllable = new ControllerMovable(rand(), objectId, renderable, 10.0f);
	Gravitable *gravitable = new Gravitable(rand(), objectId, renderable, collidable);
	Jumpable *jumpable = new Jumpable(rand(), objectId, renderable, 400.0f);

	list<PureComponent *>* charCompList = new list<PureComponent *>();
	charCompList->push_back(renderable);
	charCompList->push_back(collidable);
	charCompList->push_back(controllable);
	charCompList->push_back(gravitable);
	charCompList->push_back(jumpable);

	(GameObjectManager::getInstance()->getComponents())->push_back(renderable);
	(GameObjectManager::getInstance()->getComponents())->push_back(collidable);
	(GameObjectManager::getInstance()->getComponents())->push_back(controllable);
	(GameObjectManager::getInstance()->getComponents())->push_back(gravitable);
	(GameObjectManager::getInstance()->getComponents())->push_back(jumpable);

	(GameObjectManager::getInstance()->getObjects())->emplace(objectId, charCompList);

	return objectId;
}

// Creates a moving platform with a given point, speeds, timer, and direction to move in
void GameObjectFactory::createMovingPlatform(vector<Vector2f> points, float xSpeed, float ySpeed, float reverseTimer, Color c) {
	// A moving platform is Renderable, Collidable, and AutoMovable
	int objectId = generateRandomObjectID();

	Renderable *renderable = new Renderable(rand(), objectId, RenderType::PLATFORM, points, c);
	Collidable *collidable = new Collidable(rand(), objectId, renderable);

	Timeline *reverseTimeline = GameObjectFactory::getInstance()->createTimeline(NULL);
	float speeds[2] = { xSpeed, ySpeed };
	AutoMovable *movable = nullptr;
	if (reverseTimer > 0.01f) {
		movable = new AutoMovable(rand(), objectId, renderable, speeds, reverseTimeline, reverseTimer);
	}
	else {
		movable = new AutoMovable(rand(), objectId, renderable, speeds, nullptr, 0.0f);
	}

	list<PureComponent *>* movingCompList = new list<PureComponent *>();
	movingCompList->push_back(renderable);
	movingCompList->push_back(collidable);
	movingCompList->push_back(movable);

	(GameObjectManager::getInstance()->getComponents())->push_back(renderable);
	(GameObjectManager::getInstance()->getComponents())->push_back(collidable);
	(GameObjectManager::getInstance()->getComponents())->push_back(movable);

	(GameObjectManager::getInstance()->getObjects())->emplace(objectId, movingCompList);
}

// Creates a static platform with given platforms and some color
void GameObjectFactory::createStaticPlatform(vector<Vector2f> points, Color c) {
	// A static platform is Renderable and Collidable
	int objectId = generateRandomObjectID();

	Renderable *renderable = new Renderable(rand(), objectId, RenderType::PLATFORM, points, c);
	Collidable *collidable = new Collidable(rand(), objectId, renderable);

	list<PureComponent *>* staticCompList = new list<PureComponent *>();
	staticCompList->push_back(renderable);
	staticCompList->push_back(collidable);

	(GameObjectManager::getInstance()->getComponents())->push_back(renderable);
	(GameObjectManager::getInstance()->getComponents())->push_back(collidable);

	(GameObjectManager::getInstance()->getObjects())->emplace(objectId, staticCompList);
}

// Creates a death region, which is essentially an invisible static platform that warps the player
// upon contact.
void GameObjectFactory::createDeathRegion(vector<Vector2f> points) {
	// A death region is Renderable (with no color), Collidable, and Warpable
	int objectId = generateRandomObjectID();

	Renderable *renderable = new Renderable(rand(), objectId, RenderType::PLATFORM, points);
	Collidable *collidable = new Collidable(rand(), objectId, renderable);
	Warpable *warpable = new Warpable(rand(), objectId, collidable, WarpType::DEATH_ZONE, Vector2f(300.0f, 400.0f));

	list<PureComponent *>* deathCompList = new list<PureComponent *>();
	deathCompList->push_back(renderable);
	deathCompList->push_back(collidable);
	deathCompList->push_back(warpable);

	(GameObjectManager::getInstance()->getComponents())->push_back(renderable);
	(GameObjectManager::getInstance()->getComponents())->push_back(collidable);
	(GameObjectManager::getInstance()->getComponents())->push_back(warpable);

	(GameObjectManager::getInstance()->getObjects())->emplace(objectId, deathCompList);
}

// Creates a pair of boundaries, which will stay constant throughout the entire game
void GameObjectFactory::createBoundaries(vector<Vector2f> leftPoints, vector<Vector2f> rightPoints) {
	// Each boundary is Renderable (with no color), Collidable, and Warpable
	int objectId1 = generateRandomObjectID();

	Renderable *renderable = new Renderable(rand(), objectId1, RenderType::PLATFORM, leftPoints);
	Collidable *collidable = new Collidable(rand(), objectId1, renderable);
	Warpable *warpable = new Warpable(rand(), objectId1, collidable, WarpType::LEFT_BOUNDARY, Vector2f(0, 0));

	list<PureComponent *>* leftBoundCompList = new list<PureComponent *>();
	leftBoundCompList->push_back(renderable);
	leftBoundCompList->push_back(collidable);
	leftBoundCompList->push_back(warpable);

	(GameObjectManager::getInstance()->getComponents())->push_back(renderable);
	(GameObjectManager::getInstance()->getComponents())->push_back(collidable);
	(GameObjectManager::getInstance()->getComponents())->push_back(warpable);

	(GameObjectManager::getInstance()->getObjects())->emplace(objectId1, leftBoundCompList);

	int objectId2 = generateRandomObjectID();

	Renderable *renderable2 = new Renderable(rand(), objectId2, RenderType::PLATFORM, rightPoints);
	Collidable *collidable2 = new Collidable(rand(), objectId2, renderable2);
	Warpable *warpable2 = new Warpable(rand(), objectId2, collidable2, WarpType::RIGHT_BOUNDARY, Vector2f(0, 0));

	list<PureComponent *>* rightBoundCompList = new list<PureComponent *>();
	rightBoundCompList->push_back(renderable2);
	rightBoundCompList->push_back(collidable2);
	rightBoundCompList->push_back(warpable2);

	(GameObjectManager::getInstance()->getComponents())->push_back(renderable2);
	(GameObjectManager::getInstance()->getComponents())->push_back(collidable2);
	(GameObjectManager::getInstance()->getComponents())->push_back(warpable2);

	(GameObjectManager::getInstance()->getObjects())->emplace(objectId2, rightBoundCompList);
}

Timeline* GameObjectFactory::createTimeline(float stepSize) {
	Timeline *newTimeline = nullptr;
	if (!stepSize) {
		newTimeline = new Timeline();
	}
	else {
		newTimeline = new Timeline(stepSize);
	}
	GameObjectManager::getInstance()->getTimelines()->push_back(newTimeline);
	return newTimeline;
}