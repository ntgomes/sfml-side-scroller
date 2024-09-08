#pragma once
#include "Renderable.h"

class Collidable : public PureComponent {
private:
	Renderable *collidable;		// The pointer to the Renderable of the same object ID
	Collidable *contactShape;	// The pointer to the Collidable of another object ID
public:
	Collidable(int uuid, int parentId, Renderable *collidable);

	Rect<float> getBounds();

	void setContactShape(Collidable *contacter);

	bool collidesWith(Collidable *other);

	bool touching();

	Collidable *getContactShape();
};