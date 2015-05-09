#include <SFML/Graphics.hpp>
#include <entityx/entityx.h>
#include <iostream>
#include "Game.h"
#include "Menu.h"
#include "Utils/InputManager.h"
#include "SFMLDebugDraw.h"
#include "Utils/SystemUtils.h"
#include "Network/NetServer.h"
#include "Network/NetClient.h"

using namespace std;

void changeToGameDir()
{
	string exeDir = SystemUtils::getExecutableDirectory();
	SystemUtils::setCwd(exeDir);
#if defined(_DEBUG) || defined(DEBUG)
	SystemUtils::setCwd("../Game");
#endif
}

int main()
{
	changeToGameDir();

	NetCode::init();

	EventManager events;

	bool isServer = false;
	NetServer server(events);
	NetClient client(events);
	if (server.connect())
	{
		isServer = true;
		cout << "Server created" << endl;
		events.subscribe<SendChatEvent>(server);
	}
	else if (client.connect())
	{
		cout << "Client created" << endl;
		events.subscribe<SendChatEvent>(client);
	}
	else
	{
		cerr << "Could not create client host" << endl;
		return EXIT_FAILURE;
	}

	sf::RenderWindow window(sf::VideoMode(800, 600), "SFML works!");
	window.setKeyRepeatEnabled(false);

	InputManager inputManager(events);

	Menu menu(window, events);

	SFMLDebugDraw debugDraw(window);

	debugDraw.SetFlags(b2Draw::e_shapeBit);
	//debugDraw.AppendFlags(b2Draw::e_jointBit);
	//AppendFlags(b2Draw::e_centerOfMassBit);
	//debugDraw.AppendFlags(b2Draw::e_aabbBit);

	Game game(&window, inputManager, events, &debugDraw);

	sf::Clock clock;
	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();

			events.emit(event);
		}

		sf::Time deltaTime = clock.restart();

		window.clear();
		game.update(deltaTime.asSeconds());
		menu.draw();
		window.display();

		if (isServer)
			server.update();
		else
			client.update();
	}

	return EXIT_SUCCESS;
}