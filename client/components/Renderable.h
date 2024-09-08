#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "PureComponent.h"

using namespace sf;

/// Enum denoting which type of render the Renderable will be.
/// This will be important for the constructor.
enum RenderType {
	CHARACTER,
	PLATFORM
};

class Renderable : public PureComponent {
private:
	ConvexShape render;
	Color color;
	RenderType rendType;
public:
	Renderable(int uuid, int parentId, RenderType type, std::vector<Vector2f> typeArgs, Color rendColor = Color(0, 0, 0, 0));

	ConvexShape* getRender();

	Vector2f getPosition();
	void setPosition(Vector2f newPos);

	Color getColor();
	void setColor(Color newColor);

	RenderType getRenderType();
};