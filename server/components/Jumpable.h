#pragma once
#include "Timeline.h"
#include "Renderable.h"

class Jumpable : public PureComponent {
private:
	Renderable *jumper;
	float speed;			// Jump speed
	Timeline *jumpTimer;
public:
	Jumpable(int uuid, int parentId, Renderable *jumper, float speed);

	bool isJumping();

	// Only called is W is pressed and jumper is touching something
	void prepareToJump();

	// GameObjectManager will take care of deciding if a jumpable object is touching something
	void jump(Timeline *movTimer);

	float getJumpSpeed();
	void setJumpSpeed(float newSpeed);
};