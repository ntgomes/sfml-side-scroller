#pragma once
#include "Timeline.h"
#include "Collidable.h"

class Gravitable : public PureComponent {
private:
	Renderable *r;
	Collidable *c;
	float force;
public:
	Gravitable(int uuid, int parentId, Renderable *rend, Collidable *col, float force = 200.0f);

	bool fall(Timeline *movTimer);

	void setGravity(float newForce);
};
