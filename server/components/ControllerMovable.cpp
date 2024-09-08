#include "ControllerMovable.h"

ControllerMovable::ControllerMovable(int uuid, int parentId, Renderable *controller, float speed) : PureComponent(uuid, parentId, ComponentType::CONTROLLERMOVABLE) {
	this->controller = controller;
	this->speed = speed;
}

void ControllerMovable::move(MoveDirection dir, Timeline *movTimer) {
	float movTimeQuanta = abs(movTimer->getTime());
	if (dir == MoveDirection::LEFT)
		controller->getRender()->move(-1 * this->speed * movTimeQuanta, 0);
	if (dir == MoveDirection::RIGHT)
		controller->getRender()->move(this->speed * movTimeQuanta, 0);
}

float ControllerMovable::getSpeed() {
	return this->speed;
}

void ControllerMovable::setSpeed(float newSpeed) {
	this->speed = newSpeed;
}