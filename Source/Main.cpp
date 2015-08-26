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
#include "Events/ClientStateEvent.h"
#include <format.h>
#include "Events/ForceDisconnectEvent.h"
#include "Events/PreloadEvent.h"
#include "Utils/Logging/Logger.h"
#include "Utils/Random.h"

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
	sf::View *menuView = nullptr;
	sf::View *screenView = nullptr;
	sf::View *gameView = nullptr;
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
	Random::randomize();
	changeToGameDir();

	Logger::run();
	Logger::showErrors();

	NetCode::init();

	GameGlobals::events = &m_events;

	sf::RenderWindow window(sf::VideoMode(800, 600), "SFML works!");
	window.setVerticalSyncEnabled(true);

	GameGlobals::window = &window;
	
	window.setKeyRepeatEnabled(false);

	InputManager inputManager;
	GameGlobals::input = &inputManager;

	Menu menu;

	AssetManager assetManager;
	GameGlobals::assetManager = &assetManager;

	m_events.subscribe<ExitEvent>(*this);
	m_events.subscribe<CreateGameEvent>(*this);
	m_events.subscribe<JoinGameEvent>(*this);
	m_events.subscribe<ForceDisconnectEvent>(*this);
	m_events.subscribe<PreloadEvent>(*this);

	sf::View menuView(sf::FloatRect(0, 0, 800, 600));
	sf::View screenView(sf::FloatRect(0, 0, 800, 600));
	GameGlobals::menuView = &menuView;
	GameGlobals::screenView = &screenView;

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
				if (GameGlobals::game)
					GameGlobals::game->refreshView();
				menuView.setSize(event.size.width, event.size.height);
				screenView.setSize(event.size.width, event.size.height);
				screenView.setCenter(event.size.width / 2.0f, event.size.height / 2.0f);
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
			auto dt = deltaTime.asSeconds();
			// Limit dt to 100ms
			if (dt > 0.1f) dt = 0.1f;
			GameGlobals::game->update(dt);
		}
		else if (!assetManager.preloadsDone())
			assetManager.preloadNext();

		window.setView(menuView);
		menu.draw();
		
		window.display();

		if (m_forceDisconnect)
			disconnect();
		else if (m_server)
			m_server->update(deltaTime.asSeconds());
		else if (m_client)
			m_client->update(deltaTime.asSeconds());
	}

	// Shutdown logger
	Logger::showErrors();
	Logger::shutdown();
	Logger::getLogThread().join();

	return EXIT_SUCCESS;
}

void Main::receive(const CreateGameEvent& evt)
{
	disconnect();
	if (evt.online)
	{
		m_server = make_unique<NetServer>();
		if (m_server->connect(evt))
		{
			cout << "Server created" << endl;
			GameGlobals::game = make_unique<ServerGame>();
			auto *game = (LocalGame *)GameGlobals::game.get();
			game->init(evt.width, evt.height);
			game->initPlayers(evt.players);
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
		auto *game = (LocalGame *)GameGlobals::game.get();
		game->init(evt.width, evt.height);
		game->initPlayers(evt.players);
		game->resetEntities();
	}
}

void Main::receive(const JoinGameEvent& evt)
{
	disconnect();
	m_client = make_unique<NetClient>();
	if (m_client->connect(evt.host, evt.port))
	{
		GameGlobals::events->emit<ClientStateEvent>(fmt::format("Connecting to {}:{}", evt.host, evt.port), ClientState::CONNECTING);
		GameGlobals::game = make_unique<ClientGame>();
	}
	else
	{
		m_client.reset();
		GameGlobals::events->emit<ClientStateEvent>("Could not create client host", ClientState::DISCONNECTED);
	}
}

void Main::receive(const ForceDisconnectEvent& evt)
{
	m_forceDisconnect = true;
}

void Main::receive(const PreloadEvent& evt)
{
	if (evt.progress == evt.total)
	{
		AnimatorManager::init();

		// Create dummy local game
		std::vector<CreateGamePlayerInfo> players;
		players.push_back(CreateGamePlayerInfo("Stan", CreateGamePlayerType::LOCAL));
		players.push_back(CreateGamePlayerInfo("Kenny", CreateGamePlayerType::COMPUTER));
		players.push_back(CreateGamePlayerInfo("Kyle", CreateGamePlayerType::COMPUTER));
		players.push_back(CreateGamePlayerInfo("Cartman", CreateGamePlayerType::COMPUTER));
		m_events.emit<CreateGameEvent>(21, 21, players);
	}
}

void Main::disconnect()
{
	m_forceDisconnect = false;
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