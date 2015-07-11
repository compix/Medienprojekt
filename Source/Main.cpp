#include "Main.h"
#include "Game.h"
#include "Menu/Menu.h"
#include "Utils/InputManager.h"
#include "Utils/SystemUtils.h"
#include "Utils/make_unique.h"
#include "GameGlobals.h"
#include <SFML/Graphics.hpp>
#include "Components/InputComponent.h"
#include "Utils/AssetManagement/TextureLoader.h"
#include "Utils/AssetManagement/TexturePacker.h"
#include "Utils/AssetManagement/AssetManager.h"
#include "Animation/AnimatorManager.h"
#include "Components/LocalInputComponent.h"
#include "Components/AIComponent.h"
#include "Components/FreeSlotComponent.h"
#include "Events/ConnectionStateEvent.h"
#include <format.h>
#include "Events/ForceDisconnectEvent.h"

using namespace std;

namespace GameGlobals
{
	sf::RenderWindow *window = nullptr;
	InputManager *input = nullptr;
	EventManager *events = nullptr;
	EntityManager *entities = nullptr;
	EntityFactory *entityFactory = nullptr;
	AssetManager *assetManager = nullptr;
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

	GameGlobals::events = &m_events;

	sf::RenderWindow window(sf::VideoMode(800, 600), "SFML works!");

	GameGlobals::window = &window;
	
	window.setKeyRepeatEnabled(false);

	InputManager inputManager;
	GameGlobals::input = &inputManager;

	Menu menu;

	AssetManager assetManager;
	GameGlobals::assetManager = &assetManager;

	AnimatorManager::init();

	m_events.subscribe<ExitEvent>(*this);
	m_events.subscribe<CreateGameEvent>(*this);
	m_events.subscribe<JoinGameEvent>(*this);
	m_events.subscribe<ForceDisconnectEvent>(*this);

	// Create dummy local game
	std::vector<CreateGamePlayerInfo> players;
	players.push_back(CreateGamePlayerInfo("Stan", CreateGamePlayerType::LOCAL));
	players.push_back(CreateGamePlayerInfo("Kenny", CreateGamePlayerType::COMPUTER));
	players.push_back(CreateGamePlayerInfo("Kyle", CreateGamePlayerType::LOCAL));
	players.push_back(CreateGamePlayerInfo("Cartman", CreateGamePlayerType::LOCAL));
	m_events.emit<CreateGameEvent>(21, 21, players);

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
				GameGlobals::game->refreshView();
				menuView.setSize(event.size.width, event.size.height);
			} else if (event.type == sf::Event::MouseMoved && GameGlobals::game)
			{
				GameGlobals::game->setMousePos(sf::Vector2i(event.mouseMove.x, event.mouseMove.y));
			}

			m_events.emit(event);
		}

		sf::Time deltaTime = clock.restart();

		window.clear();
		if (GameGlobals::game) {
			window.setView(GameGlobals::game->getView());
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

	return EXIT_SUCCESS;
}

void Main::initPlayers(const std::vector<CreateGamePlayerInfo>& players)
{
	int i = 0;
	ComponentHandle<InputComponent> input;
	for (Entity entity : GameGlobals::entities->entities_with_components(input))
	{
		if (i == players.size())
			entity.destroy();
		else
		{
			//			player->name = evt.names[i]; // fixme
			switch (players[i].type)
			{
			case CreateGamePlayerType::LOCAL: entity.assign<LocalInputComponent>(i); break;
			case CreateGamePlayerType::COMPUTER: entity.assign<AIComponent>(); break;
			case CreateGamePlayerType::CLIENT: entity.assign<FreeSlotComponent>(); break;
			}
			i++;
		}
	}
}

void Main::receive(const CreateGameEvent& evt)
{
	disconnect();
	if (evt.online)
	{
		m_server = make_unique<NetServer>();
		if (m_server->connect(evt.host, evt.port))
		{
			cout << "Server created" << endl;
			GameGlobals::game = make_unique<ServerGame>();
			GameGlobals::game->init(evt.width, evt.height);
			initPlayers(evt.players);
		}
		else
		{
			m_server.reset();
			cerr << "Could not create server host" << endl;
			GameGlobals::events->emit<ExitEvent>(); //fixme: show error to user
		}
	}
	else
	{
		GameGlobals::game = make_unique<LocalGame>();
		GameGlobals::game->init(evt.width, evt.height);
		initPlayers(evt.players);
	}
}

void Main::receive(const JoinGameEvent& evt)
{
	disconnect();
	m_client = make_unique<NetClient>();
	if (m_client->connect(evt.host, evt.port))
	{
		GameGlobals::events->emit<ConnectionStateEvent>(fmt::format("Connecting to {}:{}", evt.host, evt.port));
		GameGlobals::game = make_unique<ClientGame>();
	}
	else
	{
		m_client.reset();
		GameGlobals::events->emit<ConnectionStateEvent>("Could not create client host");
	}
}

void Main::receive(const ForceDisconnectEvent& evt)
{
	disconnect();
}

void Main::disconnect()
{
	if (m_server)
		m_server.reset();
	if (m_client)
		m_client.reset();
	if (GameGlobals::game)
		GameGlobals::game.reset();
}

int main()
{
	Main instance;
	return instance.run();
}