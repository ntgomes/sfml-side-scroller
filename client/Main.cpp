#pragma warning (disable : 4996)
#define NOMINMAX
#include <SFML/Graphics.hpp>
#include <iostream>
#include <cerrno>
#include <list>
#include <zmq.hpp>
#include "Renderable.h"

using namespace sf;
using namespace std;

int main() {
	// First, create a PUSH socket that will set up a relationship with the server's PULL socket
	zmq::context_t push_context(1);
	zmq::socket_t push_socket(push_context, ZMQ_PUSH);
	push_socket.connect("tcp://127.0.0.1:5555");

	// Friendly/affirmative messaage
	cout << "Game server found" << endl;

	// Ping the game server's PULL socket to let it know that a new player entered
	zmq::message_t reply(3);
	memcpy(reply.data(), "NEW", 3);
	push_socket.send(reply);

	// Then, connect to the PUB socket as a subscriber
	zmq::context_t sub_context(1);
	zmq::socket_t sub_socket(sub_context, ZMQ_SUB);
	sub_socket.connect("tcp://127.0.0.1:8888");
	sub_socket.setsockopt(ZMQ_SUBSCRIBE, "A", 1);

	// Finally, render the window
	RenderWindow window_client(VideoMode(800, 600), "SFML Client", Style::Resize);

	// This client's unique character ID given from the server (-1 as default)
	int char_id = -1;

	// Bool for determining if this instance is closable or not
	bool closable = true;

	while (window_client.isOpen()) {

		// Get first subscriber message: the envelope
		zmq::message_t env;
		sub_socket.recv(&env);

		// Get second subscriber message: the formatted players string
		zmq::message_t players_msg;
		sub_socket.recv(&players_msg);
		string rendString = string(static_cast<char *>(players_msg.data()), players_msg.size());

		list<Renderable *> renders;

		// Start parsing the formatted renderable string
		stringstream scanner(rendString);
		string current;

		// Skip the initial space
		getline(scanner, current, ' ');
		while (getline(scanner, current, ' ')) {
			if (current == "Character") {
				// Get the x position
				getline(scanner, current, ' ');
				float xPos = stof(current);
				// Get the y position
				getline(scanner, current, ' ');
				float yPos = stof(current);
				// Get the color
				getline(scanner, current, ' ');
				Color temp(stoll(current));

				// With the x and y, the new character can be created and its position can be set
				Renderable *render = new Renderable(0, 0, RenderType::CHARACTER, vector<Vector2f>({ Vector2f(30.0f, 50.0f) }));
				render->setPosition(Vector2f(xPos, yPos));
				render->setColor(temp);
				renders.push_back(render);

				if (char_id == -1) {
					getline(scanner, current, ' ');
					char_id = stoi(current);
				}
			}
			else if (current == "Platform") {
				// Get the x position
				getline(scanner, current, ' ');
				float xPos = stof(current);
				// Get the y position
				getline(scanner, current, ' ');
				float yPos = stof(current);
				// Get the point count
				getline(scanner, current, ' ');
				int pointCount = stoi(current);

				vector<Vector2f> temp;

				for (int i = 0; i < pointCount; i++) {
					getline(scanner, current, ' ');
					float xVertex = stof(current);
					getline(scanner, current, ' ');
					float yVertex = stof(current);
					temp.push_back(Vector2f(xVertex, yVertex));
				}

				getline(scanner, current, ' ');
				Color rendColor(stoll(current));

				Renderable *render = new Renderable(0, 0, RenderType::PLATFORM, temp, rendColor);
				render->setPosition(Vector2f(xPos, yPos));
				renders.push_back(render);
			}
		}

		Event event;
		while (window_client.pollEvent(event)) {
			switch (event.type) {
			case Event::Resized:
				// Resize happens in the backend under the event system
				break;
			case Event::GainedFocus:
				// If this client window gains focus, send the message to the PULL socket that the active player is the character tied to this client by ID
				if (char_id != -1) {
					string turn_str = to_string(char_id);
					zmq::message_t notice(4);
					memcpy(notice.data(), "TURN", 4);
					push_socket.send(notice, ZMQ_SNDMORE);

					zmq::message_t reply(turn_str.size());
					memcpy(reply.data(), turn_str.c_str(), turn_str.size());
					push_socket.send(reply);
				}
				closable = true;
				break;
			case Event::LostFocus:
				// If this client window loses focus, it cannot close when X is pressed
				closable = false;
				break;
			}

		}

		// Close the game if X is pressed
		if (Keyboard::isKeyPressed(Keyboard::Key::X) && closable) {
			string end_str = to_string(char_id);
			zmq::message_t notice(3);
			memcpy(notice.data(), "BYE", 3);
			push_socket.send(notice, ZMQ_SNDMORE);

			zmq::message_t reply(end_str.size());
			memcpy(reply.data(), end_str.c_str(), end_str.size());
			push_socket.send(reply);

			window_client.close();
		}

		window_client.clear();
		for (Renderable *r : renders) {
			window_client.draw(*(r->getRender()));
		}
		window_client.display();
	}

}