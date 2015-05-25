#include "Main.h"
#include "Game.h"
#include "Menu/Menu.h"
#include "Utils/InputManager.h"
#include "Utils/SystemUtils.h"
#include "Utils/make_unique.h"
#include "GameGlobals.h"
#include <SFML/Graphics.hpp>

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

int Main::run()
{
	changeToGameDir();

	NetCode::init();

	EventManager events;
	GameGlobals::events = &events;

	sf::RenderWindow window(sf::VideoMode(800, 600), "SFML works!");
	GameGlobals::window = &window;
	
	window.setKeyRepeatEnabled(false);

	InputManager inputManager;
	GameGlobals::input = &inputManager;

	Menu menu;

	TextureLoader textureLoader;
	textureLoader.loadAllFromJson("Assets/json/textures.json");
	GameGlobals::textures = &textureLoader;

	events.subscribe<ExitEvent>(*this);
	events.subscribe<CreateLocalGameEvent>(*this);
	events.subscribe<CreateServerEvent>(*this);
	events.subscribe<JoinServerEvent>(*this);

	// Create dummy local game
	std::vector<std::string> names;
	names.push_back("Stan");
	names.push_back("Kenny");
	names.push_back("Kyle");
	names.push_back("Cartman");
	events.emit<CreateLocalGameEvent>(21, 21, names);

	sf::View gameView(sf::FloatRect(0, 0, 800, 600));
	sf::View menuView(sf::FloatRect(0, 0, 800, 600));

	sf::Clock clock;
	while (window.isOpen() && m_running)
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
			} else if (event.type == sf::Event::MouseMoved && GameGlobals::game)
			{
				GameGlobals::game->setMousePos(sf::Vector2f((float)event.mouseMove.x, (float)event.mouseMove.y));
			}

			events.emit(event);
		}

		sf::Time deltaTime = clock.restart();

		window.clear();
		if (GameGlobals::game) {
			window.setView(gameView);
			GameGlobals::game->update(deltaTime.asSeconds());
		}

		window.setView(menuView);
		menu.draw();
		window.display();

		if (m_server)
			m_server->update();
		else if (m_client)
			m_client->update();
	}
	disconnect();

	return EXIT_SUCCESS;
}

void Main::receive(const CreateLocalGameEvent& evt)
{
	disconnect();
	GameGlobals::game = make_unique<LocalGame>();
	GameGlobals::game->init(evt.width, evt.height);
}

void Main::receive(const CreateServerEvent& evt)
{
	disconnect();
	m_server = make_unique<NetServer>();
	if (m_server->connect())
	{
		cout << "Server created" << endl;
		GameGlobals::game = make_unique<ServerGame>();
		GameGlobals::game->init(evt.width, evt.height);
	}
	else
	{
		m_server.reset();
		cerr << "Could not create client host" << endl;
		exit(EXIT_FAILURE); //fixme: show error to user
	}
}

void Main::receive(const JoinServerEvent& evt)
{
	disconnect();
	m_client = make_unique<NetClient>();
	if (m_client->connect())
	{
		cout << "Client created" << endl;
		GameGlobals::game = make_unique<ClientGame>();
	}
	else
	{
		m_client.reset();
		cerr << "Could not create client host" << endl;
		exit(EXIT_FAILURE); //fixme: show error to user
	}
}

void Main::disconnect()
{
	m_server.reset();
	m_client.reset();
	GameGlobals::game.reset();
}

int main()
{
	Main instance;
	return instance.run();
}