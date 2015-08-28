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

	sf::RenderWindow window(sf::VideoMode(GameConstants::WINDOW_WIDTH, GameConstants::WINDOW_HEIGHT), GameConstants::WINDOW_TITLE);
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

	sf::View menuView(sf::FloatRect(0, 0, float(GameConstants::MENU_WIDTH), float(GameConstants::MENU_HEIGHT)));
	sf::View screenView(sf::FloatRect(0, 0, float(GameConstants::WINDOW_WIDTH), float(GameConstants::WINDOW_HEIGHT)));
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
				onResize(static_cast<float>(event.size.width), static_cast<float>(event.size.height));
			} else if (event.type == sf::Event::MouseMoved && GameGlobals::game)
			{
				GameGlobals::game->setMousePos(sf::Vector2i(event.mouseMove.x, event.mouseMove.y));
			}
			else if (event.type == sf::Event::KeyPressed && event.key.alt && event.key.code == sf::Keyboard::Return)
			{
				toggleFullscreen();
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

void Main::toggleFullscreen()
{
	m_fullscreen = !m_fullscreen;
	if (m_fullscreen)
	{
		auto mode = sf::VideoMode::getDesktopMode();
		GameGlobals::window->create(mode, "", sf::Style::Fullscreen);
		onResize(static_cast<float>(mode.width), static_cast<float>(mode.height));
	}
	else
	{
		auto mode = sf::VideoMode(GameConstants::WINDOW_WIDTH, GameConstants::WINDOW_HEIGHT);
		GameGlobals::window->create(mode, GameConstants::WINDOW_TITLE);
		onResize(static_cast<float>(mode.width), static_cast<float>(mode.height));
	}
}

void Main::onResize(float width, float height)
{
	fitViewInto(*GameGlobals::menuView, float(GameConstants::MENU_WIDTH), float(GameConstants::MENU_HEIGHT), width, height);
	GameGlobals::screenView->setSize(width, height);
	GameGlobals::screenView->setCenter(width / 2.0f, height / 2.0f);
}

int main()
{
	Main instance;
	return instance.run();
}