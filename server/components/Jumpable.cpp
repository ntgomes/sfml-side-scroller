#include "GameObjectFactory.h"
#include "Jumpable.h"

Jumpable::Jumpable(int uuid, int parentId, Renderable *jumper, float speed) : PureComponent(uuid, parentId, ComponentType::JUMPABLE) {
	this->jumper = jumper;
	this->speed = speed;
	jumpTimer = nullptr;
}

bool Jumpable::isJumping() {
	return jumpTimer != nullptr;
}

// Only called is W is pressed and jumper is touching something
void Jumpable::prepareToJump() {
	jumpTimer = GameObjectFactory::getInstance()->createTimeline(NULL);
}

// GameObjectManager will take care of deciding if a jumpable object is touching something
void Jumpable::jump(Timeline *movTimer) {
	float movTimeQuanta = abs(movTimer->getTime());
	if (jumpTimer != nullptr) {	// Keep executing this block until jumpTimer is set back to nullptr
		float jumpTimeQuanta = abs(jumpTimer->getTime());
		if (jumpTimeQuanta < 2.0f) {
			jumper->getRender()->move(0.0f, -1 * this->speed * movTimeQuanta);
		}
		else {
			delete jumpTimer;
			jumpTimer = nullptr;
		}
		if (!Keyboard::isKeyPressed(Keyboard::Key::W) && jumpTimeQuanta < 2.0f) {
			delete jumpTimer;
			jumpTimer = nullptr;
		}
	}
}

float Jumpable::getJumpSpeed() {
	return this->speed;
}

void Jumpable::setJumpSpeed(float newSpeed) {
	this->speed = newSpeed;
}