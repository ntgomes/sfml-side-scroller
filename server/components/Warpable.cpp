#include "GameObjectManager.h"
#include "Warpable.h"

Warpable::Warpable(int uuid, int parentId, Collidable *c, WarpType type, Vector2f warpPoint) : PureComponent(uuid, parentId, ComponentType::WARPABLE) {
	this->col = c;
	this->type = type;
	this->warpPoint = warpPoint;
}

void Warpable::killCharacter() {
	if (col->touching()) {
		Collidable *temp = col->getContactShape();
		int contactShapeId = temp->getObjectId();
		for (PureComponent *p : *(GameObjectManager::getInstance()->getObjects()->at(contactShapeId))) {
			if (p->getType() == ComponentType::RENDERABLE) {
				GameEvent *deathEvent = new GameEvent(GameEventManager::getInstance()->getCurrentEventTime(), 0, EventType::CHARACTER_DEATH, 1, static_cast<Renderable *>(p));
				GameEventManager::getInstance()->raiseEvent(deathEvent);
				break;
			}
		}
	}
}

void Warpable::leftBoundTrigger(RenderWindow *window) {
	if (col->touching()) {
		GameObjectManager *instance = GameObjectManager::getInstance();
		Renderable *temp = nullptr;
		for (PureComponent *p : *instance->getObjects()->at(col->getContactShape()->getObjectId())) {
			if (p->getType() == ComponentType::RENDERABLE) {
				temp = static_cast<Renderable *>(p);
				break;
			}
		}
		if (temp->getRenderType() == RenderType::CHARACTER) {
			Vector2f *oldLeftPosition = nullptr;
			Vector2f *oldRightPosition = nullptr;
			for (PureComponent *p : *instance->getComponents()) {
				if (p->getType() == ComponentType::WARPABLE) {
					int boundaryId = p->getObjectId();
					for (PureComponent *p2 : *instance->getObjects()->at(boundaryId)) {
						if (p2->getType() == ComponentType::RENDERABLE) {
							if (static_cast<Warpable *>(p)->getType() == WarpType::LEFT_BOUNDARY) {
								oldLeftPosition = &static_cast<Renderable *>(p2)->getPosition();
							}
							else if (static_cast<Warpable *>(p)->getType() == WarpType::RIGHT_BOUNDARY) {
								oldRightPosition = &static_cast<Renderable *>(p2)->getPosition();
							}
						}
						break;
					}
				}
			}
			int i = 950;
			instance->setTransitioning(true);
			col->setContactShape(nullptr);
			temp->getRender()->move(-50.0f, 0.0f);
			for (Renderable *r : instance->getRenderables()) {
				if (r->getRenderType() == RenderType::CHARACTER) {
					r->setPosition(temp->getPosition());
				}
			}
			while (i > 0) {
				for (PureComponent *p : *instance->getComponents()) {
					if (p->getType() == ComponentType::RENDERABLE) {
						static_cast<Renderable *>(p)->getRender()->move(0.8f, 0.0f);
					}
				}
				instance->renderObjects(window);
				i--;
			}
			for (PureComponent *p : *instance->getComponents()) {
				if (p->getType() == ComponentType::WARPABLE) {
					int boundaryId = p->getObjectId();
					for (PureComponent *p2 : *instance->getObjects()->at(boundaryId)) {
						if (p2->getType() == ComponentType::RENDERABLE) {
							if (static_cast<Warpable *>(p)->getType() == WarpType::LEFT_BOUNDARY) {
								static_cast<Renderable *>(p2)->setPosition(*oldLeftPosition);
							}
							else if (static_cast<Warpable *>(p)->getType() == WarpType::RIGHT_BOUNDARY) {
								static_cast<Renderable *>(p2)->setPosition(*oldRightPosition);
							}
						}
						break;
					}
				}
			}
			instance->setTransitioning(false);
		}
	}
}

void Warpable::rightBoundTrigger(RenderWindow *window) {
	if (col->touching()) {
		GameObjectManager *instance = GameObjectManager::getInstance();
		Renderable *temp = nullptr;
		for (PureComponent *p : *instance->getObjects()->at(col->getContactShape()->getObjectId())) {
			if (p->getType() == ComponentType::RENDERABLE) {
				temp = static_cast<Renderable *>(p);
				break;
			}
		}
		if (temp->getRenderType() == RenderType::CHARACTER) {
			Vector2f *oldLeftPosition = nullptr;
			Vector2f *oldRightPosition = nullptr;
			for (PureComponent *p : *instance->getComponents()) {
				if (p->getType() == ComponentType::WARPABLE) {
					int boundaryId = p->getObjectId();
					for (PureComponent *p2 : *instance->getObjects()->at(boundaryId)) {
						if (p2->getType() == ComponentType::RENDERABLE) {
							if (static_cast<Warpable *>(p)->getType() == WarpType::LEFT_BOUNDARY) {
								oldLeftPosition = &static_cast<Renderable *>(p2)->getPosition();
							}
							else if (static_cast<Warpable *>(p)->getType() == WarpType::RIGHT_BOUNDARY) {
								oldRightPosition = &static_cast<Renderable *>(p2)->getPosition();
							}
						}
						break;
					}
				}
			}
			int i = 950;
			instance->setTransitioning(true);
			temp->getRender()->move(50.0f, 0.0f);
			col->setContactShape(nullptr);
			for (Renderable *r : instance->getRenderables()) {
				if (r->getRenderType() == RenderType::CHARACTER) {
					r->setPosition(temp->getPosition());
				}
			}
			while (i > 0) {
				for (PureComponent *p : *instance->getComponents()) {
					if (p->getType() == ComponentType::RENDERABLE) {
						static_cast<Renderable *>(p)->getRender()->move(-0.8f, 0.0f);
					}
				}
				instance->renderObjects(window);
				i--;
			}
			for (PureComponent *p : *instance->getComponents()) {
				if (p->getType() == ComponentType::WARPABLE) {
					int boundaryId = p->getObjectId();
					for (PureComponent *p2 : *instance->getObjects()->at(boundaryId)) {
						if (p2->getType() == ComponentType::RENDERABLE) {
							if (static_cast<Warpable *>(p)->getType() == WarpType::LEFT_BOUNDARY) {
								static_cast<Renderable *>(p2)->setPosition(*oldLeftPosition);
							}
							else if (static_cast<Warpable *>(p)->getType() == WarpType::RIGHT_BOUNDARY) {
								static_cast<Renderable *>(p2)->setPosition(*oldRightPosition);
							}
						}
						break;
					}
				}
			}
			instance->setTransitioning(false);
		}
	}
}

WarpType Warpable::getType() {
	return this->type;
}