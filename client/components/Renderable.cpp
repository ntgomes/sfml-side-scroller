#include "Renderable.h"

Renderable::Renderable(int uuid, int parentId, RenderType type, std::vector<Vector2f> typeArgs, Color rendColor) : PureComponent(uuid, parentId, ComponentType::RENDERABLE) {
	if (type == RenderType::CHARACTER) {
		// typeArgs is treated as a size dimension
		RectangleShape temp(typeArgs[0]);
		render.setPointCount(4);

		// Set the new character's color to some random color
		srand(time(NULL));
		int red = rand() % 255;
		int blue = rand() % 255;
		int green = rand() % 255;
		Color *charColor = new Color(red, green, blue);
		this->color = *charColor;

		// Copy the vertices of the temp RectangleShape to the ConvexShape field
		for (int i = 0; i < 4; i++)
			render.setPoint(i, temp.getPoint(i));

		render.setFillColor(*charColor);
		rendType = type;
	}
	else {
		// typeArgs is treated as the points
		render.setPointCount(typeArgs.size());
		render.setFillColor(rendColor);
		this->color = rendColor;

		for (int i = 0; i < typeArgs.size(); i++)
			render.setPoint(i, typeArgs[i]);

		rendType = type;
	}
}

ConvexShape* Renderable::getRender() {
	return &render;
}

Vector2f Renderable::getPosition() {
	return this->getRender()->getPosition();
}

void Renderable::setPosition(Vector2f newPos) {
	this->getRender()->setPosition(newPos);
}

Color Renderable::getColor() {
	return this->color;
}

void Renderable::setColor(Color newColor) {
	this->getRender()->setFillColor(newColor);
}

RenderType Renderable::getRenderType() {
	return this->rendType;
}