#include "PureComponent.h"

PureComponent::PureComponent(int uuid, int objectId, ComponentType type) {
	this->uuid = uuid;
	this->objectId = objectId;
	this->compType = type;
}

int PureComponent::getUUID() {
	return this->uuid;
}

int PureComponent::getObjectId() {
	return this->objectId;
}

ComponentType PureComponent::getType() {
	return this->compType;
}