#include <SFML/Graphics.hpp>
#include <entityx/entityx.h>
#include <iostream>
#include "Game.h"
#include "Menu/Menu.h"
#include "Utils/InputManager.h"
#include "SFMLDebugDraw.h"
#include "Utils/SystemUtils.h"
#include "Network/NetServer.h"
#include "Network/NetClient.h"
#include "Events/ExitEvent.h"

using namespace std;

namespace GameGlobals
{
	sf::RenderWindow *window = nullptr;
	InputManager *input = nullptr;
	EventManager *events = nullptr;
	EntityManager *entities = nullptr;
	EntityFactory *entityFactory = nullptr;
	TextureLoader *textures = nullptr;
	unique_ptr<Game> game;
};

void changeToGameDir()
{
	string exeDir = SystemUtils::getExecutableDirectory();
	SystemUtils::setCwd(exeDir);
#if defined(_DEBUG) || defined(DEBUG)
	SystemUtils::setCwd("../Game");
#endif
}

struct ExitListener : public Receiver<ExitListener>
{
	bool triggered = false;
	void receive(const ExitEvent& evt)
	{
		triggered = true;
	}
};

int main()
{
	changeToGameDir();

	NetCode::init();

	EventManager events;
	GameGlobals::events = &events;

	bool isServer = false;
	NetServer server;
	NetClient client;
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
	GameGlobals::window = &window;
	
	window.setKeyRepeatEnabled(false);

	InputManager inputManager;
	GameGlobals::input = &inputManager;

	Menu menu;
	TextureLoader textureLoader;
	textureLoader.loadAllFromJson("Assets/json/textures.json");
	GameGlobals::textures = &textureLoader;

	GameGlobals::game = make_unique<Game>();

	ExitListener exitListener;
	events.subscribe<ExitEvent>(exitListener);

	sf::View gameView(sf::FloatRect(0, 0, 800, 600));
	sf::View menuView(sf::FloatRect(0, 0, 800, 600));

	sf::Clock clock;
	while (window.isOpen() && !exitListener.triggered)
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
			else if (event.type == sf::Event::Resized)
			{
				gameView.setSize(event.size.width, event.size.height);
				menuView.setSize(event.size.width, event.size.height);
			}
			else if (event.type == sf::Event::MouseMoved && GameGlobals::game)
			{
				GameGlobals::game->setMousePos(sf::Vector2f((float)event.mouseMove.x, (float)event.mouseMove.y));
			}

			events.emit(event);
		}

		sf::Time deltaTime = clock.restart();

		window.clear();
		window.setView(gameView);
		GameGlobals::game->update(deltaTime.asSeconds());

		window.setView(menuView);
		menu.draw();
		window.display();

		if (isServer)
			server.update();
		else
			client.update();
	}

	GameGlobals::game.reset();

	return EXIT_SUCCESS;
}