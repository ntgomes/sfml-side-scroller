#pragma once
#include "Timeline.h"
#include "Renderable.h"

class AutoMovable : public PureComponent {
private:
	Renderable *render;
	float xSpeed;
	float ySpeed;
	float reverseTime;
	Timeline *reverseTimer;		// The timer at which AutoMovable reverses velocity

public:
	AutoMovable(int uuid, int parentId, Renderable *render, float speeds[], Timeline *timer, float timeToReverse);

	void move(Timeline *movTimer);

	float getXSpeed();

	float getYSpeed();

	void setXSpeed(float newX);

	void setYSpeed(float newY);
};