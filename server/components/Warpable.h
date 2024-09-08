#pragma once
#include "Collidable.h"

enum WarpType {
	DEATH_ZONE,
	LEFT_BOUNDARY,
	RIGHT_BOUNDARY,
};

class Warpable : public PureComponent {
private:
	Collidable *col;
	Vector2f warpPoint;
	WarpType type;
public:
	Warpable(int uuid, int parentId, Collidable *c, WarpType type, Vector2f warpPoint);

	void killCharacter();

	void leftBoundTrigger(RenderWindow *window);

	void rightBoundTrigger(RenderWindow *window);

	WarpType getType();
};