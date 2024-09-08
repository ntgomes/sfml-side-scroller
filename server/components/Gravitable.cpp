#include "Gravitable.h"

Gravitable::Gravitable(int uuid, int parentId, Renderable *rend, Collidable *col, float force) : PureComponent(uuid, parentId, ComponentType::GRAVITABLE) {
	this->r = rend;
	this->c = col;
	this->force = force;
}

bool Gravitable::fall(Timeline *movTimer) {
	if (!c->touching()) {
		float movTimeQuanta = abs(movTimer->getTime());
		r->getRender()->move(0.0f, force * movTimeQuanta);
		return true;
	}
	return false;
}

void Gravitable::setGravity(float newForce) {
	this->force = newForce;
}