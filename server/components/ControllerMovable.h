#pragma once
#include "Timeline.h"
#include "Renderable.h"

enum MoveDirection {
	LEFT,
	RIGHT
};

class ControllerMovable : public PureComponent {
private:
	Renderable *controller;
	float speed;			// Horizontal speed
public:
	ControllerMovable(int uuid, int parentId, Renderable *controller, float speed);

	void move(MoveDirection dir, Timeline *movTimer);

	float getSpeed();

	void setSpeed(float newSpeed);
};