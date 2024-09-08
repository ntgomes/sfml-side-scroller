#pragma warning (disable : 4996)
#define NOMINMAX
#include <iostream>
#include <zmq.hpp>
#include <SFML/System.hpp>
#include "duktape.h"
#include "ReplayManager.h"
#include "GameObjectFactory.h"

using namespace sf;
using namespace std;

int rend_size = 1;				// Number of players
int activePlayerID = -1;		// Object ID belonging to the player whose window has focus
int server_player_id;			// The object ID of the player that the server created
bool newPlayerDetected = false;	// The boolean detecting whether a new player has been detected
bool closable = true;			// Boolean denoting if this instance of the game is closable

/// Function that calls static manager to handle active character movement
void handlePlayerMovement(Timeline *movTimer) {
	GameObjectManager::getInstance()->controlActiveCharacter(activePlayerID, movTimer);
}

/// Function that calls static manager to handle gravity for all gravitable components
void handleGravity(Timeline *movTimer) {
	GameObjectManager::getInstance()->handleGravity(movTimer);
}

/// Function that calls static manager to handle renderables that can move on its own
void handleAutoMovement(Timeline *movTimer) {
	GameObjectManager::getInstance()->handleAutoMoves(movTimer);
}

/// Function that calls static manager to handle collision detection for all collidables
void handleCollisionDetection(Timeline *movTimer) {
	GameObjectManager::getInstance()->handleCollisions(movTimer);
}

void handleGameEvents() {
	GameEventManager::getInstance()->handleEvents();
}

/// Function that checks for new messages from clients
void checkForClientConnections(zmq::socket_t *recvr) {
	zmq::message_t request;
	recvr->recv(&request);
	string client_data = string(static_cast<char *>(request.data()), request.size());

	// If the string that the PULL socket recieved is "NEW"
	if (client_data == "NEW") {
		cout << "New client detected!" << endl;
		rend_size++;
		// Create a new character that will be rendered into the scene
		activePlayerID = GameObjectFactory::getInstance()->createCharacter(Vector2f(30.0f, 50.0f));
		Renderable *clientChar = nullptr;

		for (Renderable *r : GameObjectManager::getInstance()->getRenderables()) {
			if (r->getObjectId() == activePlayerID) {
				clientChar = r;
			}
		}

		Color *clientCharColor = new Color(clientChar->getColor());
		GameEvent *clientSpawnEvent = new GameEvent(GameEventManager::getInstance()->getCurrentEventTime(), 0, EventType::CHARACTER_SPAWN, 3, clientChar, clientCharColor);
		GameEventManager::getInstance()->raiseEvent(clientSpawnEvent);

		cout << "New client rendered: ID " << activePlayerID << endl;
		newPlayerDetected = true;
	}
	else if (client_data == "TURN") {
		zmq::message_t request2;
		recvr->recv(&request2);
		string id_data = string(static_cast<char *>(request2.data()), request2.size());

		activePlayerID = stoi(id_data);
	}
	else if (client_data == "BYE") {
		sleep(milliseconds(100));	// Have delay for checking X press

		zmq::message_t request3;
		recvr->recv(&request3);
		string id_data = string(static_cast<char *>(request3.data()), request3.size());

		int idToDelete = stoi(id_data);

		GameObjectManager::getInstance()->deleteObject(idToDelete);

		cout << "Client " << idToDelete << " has left the game" << endl;
		rend_size--;
	}
}

/// Function that handles sending the renderables string to all clients
void sendDataToClients(zmq::socket_t *sender) {
	if (rend_size > 1) {
		list<Renderable *> renderables = GameObjectManager::getInstance()->getRenderables();
		zmq::message_t env(1);
		memcpy(env.data(), "A", 1);
		sender->send(env, ZMQ_SNDMORE);	// Send envelope

		string rendString = "";
		for (Renderable *r : renderables) {
			if (r->getRenderType() == RenderType::CHARACTER) {
				// For character renderables, send x, y, 32-bit color, and ID
				rendString.append(" Character ");
				rendString.append(to_string(r->getPosition().x) + " ");
				rendString.append(to_string(r->getPosition().y) + " ");

				rendString.append(to_string(r->getColor().toInteger()));

				if (newPlayerDetected) {
					rendString.append(" " + to_string(activePlayerID));
					//newPlayerDetected = false;
					// Maybe get rid of the previous line? Worked without it?
				}
			}
			else if (r->getRenderType() == RenderType::PLATFORM) {
				// For platforms, send their position as x and y, their vertices, and color
				rendString.append(" Platform ");
				rendString.append(to_string(r->getPosition().x) + " ");
				rendString.append(to_string(r->getPosition().y) + " ");

				rendString.append(to_string(r->getRender()->getPointCount()) + " ");
				for (unsigned int i = 0; i < r->getRender()->getPointCount(); i++) {
					rendString.append(to_string(r->getRender()->getPoint(i).x) + " ");
					rendString.append(to_string(r->getRender()->getPoint(i).y) + " ");
				}

				rendString.append(to_string(r->getColor().toInteger()));
			}
		}

		zmq::message_t players_msg(rendString.size());
		memcpy(players_msg.data(), rendString.c_str(), rendString.size());
		sender->send(players_msg);
	}
}


int main()
{
	// First, set up the PULL server, since it will only be listening for client messages
	zmq::context_t pull_context(1);
	zmq::socket_t pull_socket(pull_context, ZMQ_PULL);
	pull_socket.bind("tcp://127.0.0.1:5555");

	// Next, set up the PUB server, so that it can solely send renderable information to the clients
	zmq::context_t pub_context(1);
	zmq::socket_t pub_socket(pub_context, ZMQ_PUB);
	pub_socket.bind("tcp://127.0.0.1:8888");

	// We want our game to work without the need for a client, so set PULL socket recieve timeout to 1 ms
	int timeout = 1;
	zmq_setsockopt(pull_socket, ZMQ_RCVTIMEO, &timeout, sizeof(int));

	// Friendly message for the starting of a server... using JavaScript!
	cout << "Server started" << endl;

	// Render the window so that it can be polled during execution
	RenderWindow window(VideoMode(800, 600), "SFML Server", Style::Resize);

	// Set the server's player ID and by default, set it to active player ID
	server_player_id = GameObjectFactory::getInstance()->createCharacter(Vector2f(30.0f, 50.0f));
	activePlayerID = server_player_id;

	Renderable *serverChar = *GameObjectManager::getInstance()->getRenderables().begin();
	Color *serverCharColor = new Color(serverChar->getColor());
	GameEvent *serverSpawnEvent = new GameEvent(GameEventManager::getInstance()->getCurrentEventTime(), 0, EventType::CHARACTER_SPAWN, 3, serverChar, serverCharColor);
	GameEventManager::getInstance()->raiseEvent(serverSpawnEvent);

	// Load game world objects with the GameObjectFactory
	GameObjectFactory::getInstance()->createStaticPlatform(vector<Vector2f>(
		{ Vector2f(-100, 550), Vector2f(450, 550), Vector2f(450, 700), Vector2f(-100, 700) }), Color::Green);
	GameObjectFactory::getInstance()->createStaticPlatform(vector<Vector2f>(
		{ Vector2f(-800, 550), Vector2f(-100, 550), Vector2f(-100, 700), Vector2f(-800, 700) }), Color::Blue);
	GameObjectFactory::getInstance()->createDeathRegion(vector<Vector2f>(
		{ Vector2f(451, 580), Vector2f(549, 580), Vector2f(549, 700), Vector2f(451, 700) }));
	GameObjectFactory::getInstance()->createStaticPlatform(vector<Vector2f>(
		{ Vector2f(550, 550), Vector2f(1800, 550), Vector2f(1800, 700), Vector2f(550, 700) }), Color::Cyan);
	GameObjectFactory::getInstance()->createMovingPlatform(vector<Vector2f>(
		{ Vector2f(500, 450), Vector2f(600, 450), Vector2f(575, 500), Vector2f(525, 500) }), 200.0f, 0.0f, 4.0f);
	GameObjectFactory::getInstance()->createMovingPlatform(vector<Vector2f>(
		{ Vector2f(150, 100), Vector2f(250, 100), Vector2f(225, 150), Vector2f(175, 150) }), 0.0f, 200.0f, 4.0f, Color::Red);
	GameObjectFactory::getInstance()->createBoundaries(vector<Vector2f>(
		{ Vector2f(10, 0), Vector2f(20, 0), Vector2f(20, 540), Vector2f(10, 540) }), vector<Vector2f>(
			{ Vector2f(780, 0), Vector2f(790, 0), Vector2f(790, 540), Vector2f(780, 540) }));
	GameObjectFactory::getInstance()->createMovingPlatform(vector<Vector2f>(
		{ Vector2f(-600, 450), Vector2f(-500, 450), Vector2f(-525, 500), Vector2f(-575, 500) }), 200.0f, 0.0f, 4.0f, Color::Magenta);

	// Create the timelines
	Timeline *globalTime = GameObjectFactory::getInstance()->createTimeline(NULL);
	Timeline *movTime = GameObjectFactory::getInstance()->createTimeline(NULL);

	cout << "Server's ID is " << server_player_id << endl;
	cout << "You can now start up clients...\n\nLOG\n-----" << endl;

	while (window.isOpen())
	{
		Event event;
		while (window.pollEvent(event)) {
			switch (event.type) {
			case Event::Resized:
				break;
			case Event::GainedFocus:
				activePlayerID = server_player_id;
				closable = true;
				break;
			case Event::LostFocus:
				closable = false;
				break;
			}
		}

		// Reset movTimer
		movTime->updateFrame();

		// Close the game if X is pressed
		if (Keyboard::isKeyPressed(Keyboard::Key::X) && closable)
			window.close();

		if (Keyboard::isKeyPressed(Keyboard::Key::N)) {
			int *nKeyCode = new int(Keyboard::Key::N);
			GameEvent *newRecordKeyEvent = new GameEvent(GameEventManager::getInstance()->getCurrentEventTime(), 0, EventType::USER_INPUT, 3, nKeyCode, nullptr, nullptr);
			GameEventManager::getInstance()->raiseEvent(newRecordKeyEvent);
		}

		if (Keyboard::isKeyPressed(Keyboard::Key::S)) {
			int *sKeyCode = new int(Keyboard::Key::S);
			GameEvent *stopRecordKeyEvent = new GameEvent(GameEventManager::getInstance()->getCurrentEventTime(), 0, EventType::USER_INPUT, 3, sKeyCode, nullptr, nullptr);
			GameEventManager::getInstance()->raiseEvent(stopRecordKeyEvent);
		}


		if (Keyboard::isKeyPressed(Keyboard::Key::R))
			ReplayManager::getInstance()->replayRecording(&window);

		Thread controllerThread(&handlePlayerMovement, movTime);
		Thread gravityThread(&handleGravity, movTime);
		Thread autoMoveThread(&handleAutoMovement, movTime);
		Thread collisionThread(&handleCollisionDetection, movTime);
		Thread newConnThread(&checkForClientConnections, &pull_socket);
		Thread sendDataThread(&sendDataToClients, &pub_socket);
		Thread handleEvents(&handleGameEvents);

		handleEvents.launch();

		newConnThread.launch();

		controllerThread.launch();
		gravityThread.launch();
		autoMoveThread.launch();
		collisionThread.launch();

		// Main thread will handle rendering due to RenderWindow being ThreadLocal
		GameObjectManager::getInstance()->renderObjects(&window);
		GameObjectManager::getInstance()->handleWarps(&window);

		// Main thread will also handle calling record() replay manager since RenderWindow is ThreadLocal
		ReplayManager::getInstance()->record();

		sendDataThread.launch();
	}

	return 0;
}