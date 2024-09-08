#pragma once
#include "GameObjectManager.h"

class GameObjectFactory {
private:
	static GameObjectFactory *instance;
	GameObjectFactory();
	int generateRandomObjectID();
public:
	static GameObjectFactory *getInstance();

	int createCharacter(Vector2f dimensions);

	void createMovingPlatform(vector<Vector2f> points, float xSpeed, float ySpeed, float reverseTime = 0.0f, Color c = Color::Yellow);

	void createStaticPlatform(vector<Vector2f> points, Color c = Color::Green);

	void createDeathRegion(vector<Vector2f> points);

	void createBoundaries(vector<Vector2f> leftPoints, vector<Vector2f> rightPoints);

	Timeline* createTimeline(float stepSize);
};