#include "ReplayManager.h"

ReplayManager::ReplayManager() {
	replayString = "";
	isRecording = false;
	replayer = nullptr;
}
ReplayManager* ReplayManager::instance;
Mutex ReplayManager::replayLock;

ReplayManager* ReplayManager::getInstance() {
	if (instance == nullptr) {
		instance = new ReplayManager();
	}
	return instance;
}

void ReplayManager::startRecording() {
	if (!isRecording) {
		isRecording = true;
		replayString = "";
	}
}

void ReplayManager::record() {
	if (isRecording) {
		replayLock.lock();
		replayString.append("| ");
		list<Renderable *> rendersOfFrame = GameObjectManager::getInstance()->getRenderables();
		for (Renderable *r : rendersOfFrame) {
			replayString.append("Renderable ");
			replayString.append(to_string(r->getObjectId()) + " ");
			replayString.append(to_string(r->getPosition().x) + " ");
			replayString.append(to_string(r->getPosition().y) + " ");
		}
		replayLock.unlock();
	}
}

void ReplayManager::stopRecording() {
	if (isRecording) {
		replayLock.lock();
		isRecording = false;
		replayLock.unlock();
	}
}

void ReplayManager::replayRecording(RenderWindow *replayWindow) {
	replayer = replayWindow;
	string beforeReplayBuffer = "";
	list<Renderable *> rendersBeforeReplay = GameObjectManager::getInstance()->getRenderables();
	for (Renderable *r : rendersBeforeReplay) {
		beforeReplayBuffer.append("Renderable ");
		beforeReplayBuffer.append(to_string(r->getObjectId()) + " ");
		beforeReplayBuffer.append(to_string(r->getPosition().x) + " ");
		beforeReplayBuffer.append(to_string(r->getPosition().y) + " ");
	}
	replayer->setFramerateLimit(240);
	// Now pause all timelines
	for (Timeline *t : *GameObjectManager::getInstance()->getTimelines()) {
		t->setScaleFactor(0);
	}
	stringstream replayScanner(replayString);
	string currentFrame;
	getline(replayScanner, currentFrame, '|'); // Skip the original pipe
	while (getline(replayScanner, currentFrame, '|')) {
		string currentToken;
		getline(replayScanner, currentToken, ' ');	// Skip the initial space
		getline(replayScanner, currentToken, ' ');	// Get what should be "Renderable"
		while (currentToken == "Renderable") {
			getline(replayScanner, currentToken, ' ');
			int currentOID = stoi(currentToken);
			getline(replayScanner, currentToken, ' ');
			float currentXPos = stof(currentToken);
			getline(replayScanner, currentToken, ' ');
			float currentYPos = stof(currentToken);

			for (Renderable *r : GameObjectManager::getInstance()->getRenderables()) {
				if (r->getObjectId() == currentOID) {
					r->setPosition(Vector2f(currentXPos, currentYPos));
				}
			}

			// Key presses in reponse to speed changes
			if (Keyboard::isKeyPressed(Keyboard::Key::F)) {
				replayer->setFramerateLimit(480);
			}
			if (Keyboard::isKeyPressed(Keyboard::Key::H)) {
				replayer->setFramerateLimit(120);
			}
			if (Keyboard::isKeyPressed(Keyboard::Key::G)) {
				replayer->setFramerateLimit(240);
			}

			GameObjectManager::getInstance()->renderObjects(replayWindow);

			getline(replayScanner, currentToken, ' ');
		}
	}
	// Now set everything back based on the beforeReplayBuffer
	stringstream beforeScanner(beforeReplayBuffer);
	string current;
	getline(beforeScanner, current, ' '); // Get the "Renderable" identifier
	while (current == "Renderable") {
		getline(beforeScanner, current, ' ');
		int currentOID = stoi(current);
		getline(beforeScanner, current, ' ');
		float currentXPos = stof(current);
		getline(beforeScanner, current, ' ');
		float currentYPos = stof(current);

		for (Renderable *r : GameObjectManager::getInstance()->getRenderables()) {
			if (r->getObjectId() == currentOID) {
				r->setPosition(Vector2f(currentXPos, currentYPos));
			}
		}

		GameObjectManager::getInstance()->renderObjects(replayWindow);

		getline(beforeScanner, current, ' ');
	}

	for (Timeline *t : *GameObjectManager::getInstance()->getTimelines()) {
		t->updateFrame();
		t->setScaleFactor(1);
	}

	replayer->setFramerateLimit(0);
	replayer = nullptr;
}