#include "AutoMovable.h"

AutoMovable::AutoMovable(int uuid, int parentId, Renderable *render, float speeds[], Timeline *timer, float timeToReverse) : PureComponent(uuid, parentId, ComponentType::AUTOMOVABLE) {
	this->render = render;
	this->xSpeed = speeds[0];
	this->ySpeed = speeds[1];
	this->reverseTimer = timer;
	this->reverseTime = timeToReverse;
}

void AutoMovable::move(Timeline *movTimer) {
	float movTimeQuanta = abs(movTimer->getTime());
	if (reverseTimer != nullptr) {
		if (abs(reverseTimer->getTime()) > reverseTime) {
			this->xSpeed *= -1;
			this->ySpeed *= -1;
			reverseTimer->updateFrame();
		}
		else {
			render->getRender()->move(xSpeed * movTimeQuanta, ySpeed * movTimeQuanta);
		}
	}
	else {	// Having no upperBound means infinite movement in one direction
		render->getRender()->move(xSpeed * movTimeQuanta, ySpeed * movTimeQuanta);
	}
}

float AutoMovable::getXSpeed() {
	return this->xSpeed;
}

float AutoMovable::getYSpeed() {
	return this->ySpeed;
}

void AutoMovable::setXSpeed(float newX) {
	this->xSpeed = newX;
}

void AutoMovable::setYSpeed(float newY) {
	this->ySpeed = newY;
}