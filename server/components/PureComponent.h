#pragma once

/// C++ doesn't have support for the instanceof operator
/// like Java has, so to fix that, PureComponent will have
/// a enum that will keep track of all possible types of
/// PureComponent children.
enum ComponentType {
	RENDERABLE,
	COLLIDABLE,
	AUTOMOVABLE,
	CONTROLLERMOVABLE,
	JUMPABLE,
	GRAVITABLE,
	WARPABLE
};

/// The base class of any component in this game object model.
class PureComponent {
private:
	int uuid;
	int objectId;
	ComponentType compType;
public:
	PureComponent(int uuid, int objectId, ComponentType type);
	int getUUID();
	int getObjectId();
	ComponentType getType();
};