#include "Collidable.h"

Collidable::Collidable(int uuid, int parentId, Renderable *collidable) : PureComponent(uuid, parentId, ComponentType::COLLIDABLE) {
	this->collidable = collidable;
	this->contactShape = nullptr;
}

Rect<float> Collidable::getBounds() {
	return collidable->getRender()->getGlobalBounds();
}

void Collidable::setContactShape(Collidable *contacter) {
	contactShape = contacter;
}

bool Collidable::collidesWith(Collidable *other) {
	bool collides = this->getBounds().intersects(other->getBounds());
	if (collides)
		setContactShape(other);
	return collides;
}

Collidable *Collidable::getContactShape() {
	return contactShape;
}

bool Collidable::touching() {
	return contactShape != nullptr;
}