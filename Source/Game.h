#pragma once
#include "entityx/entityx.h"
#include <SFML/Graphics.hpp>
#include <memory>
#include "Utils/AssetManagement/TextureLoader.h"
#include "EntityFactory.h"
#include "LayerManager.h"
#include "SFMLDebugDraw.h"
#include "Lighting/Light.h"
#include "Utils/ShaderManager.h"
#include "GameGlobals.h"
#include "ContactListener.h"

struct MenuShowEvent;
struct HoldingStatusEvent;
struct ResetGameEvent;
struct GameOverEvent;
using entityx::TimeDelta;
using entityx::EventManager;
using entityx::SystemManager;
using entityx::Receiver;
using std::unique_ptr;

class ParticleEmitter;

class Game
{
public:
	Game();
	virtual ~Game();
	virtual void init(uint8_t width, uint8_t height);

	virtual void update(TimeDelta dt);

	inline void setMousePos(sf::Vector2i mousePos)
	{
		m_mousePos = sf::Vector2f(GameGlobals::window->mapPixelToCoords(sf::Mouse::getPosition(*GameGlobals::window)));
	}

	uint8_t getWidth() { return m_width; }
	uint8_t getHeight() { return m_height; }
	virtual bool isClient() { return false; }
	virtual bool isServer() { return false; }

	void refreshView();
	inline const sf::View& getView() const { return m_view; }
protected:
	void addSystems();

	template <typename S, typename ... Args>
	std::shared_ptr<S> addSystem(Args && ... args);
protected:
	sf::View m_view;
	bool initialized = false;
	EntityManager m_entities;
	SystemManager m_systems;
    SFMLDebugDraw m_debugDraw;
    PhysixSystem* m_PhysixSystem;
	// Order depends on the order they are added with addSystem()
	std::vector<std::shared_ptr<entityx::BaseSystem>> m_orderedSystems;
	unique_ptr<EntityFactory> m_entityFactory;
	unique_ptr<LayerManager> m_layerManager;
	ContactListener listener;
	uint8_t m_width;
	uint8_t m_height;

	ShaderManager m_shaderManager;

	sf::Vector2f m_mousePos;
};

template <typename S, typename ... Args>
std::shared_ptr<S> Game::addSystem(Args&&... args)
{
	auto systemPtr = m_systems.add<S>(args...);
	m_orderedSystems.push_back(systemPtr);
	return systemPtr;
}

enum class CreateGamePlayerType;
struct CreateGamePlayerInfo;

class LocalGame : public Game, public Receiver<LocalGame>
{
public:
	LocalGame();
	~LocalGame();

	void initPlayers(const vector<CreateGamePlayerInfo> &players);
	void resetEntities();

	void update(TimeDelta dt) override;
	void receive(const GameOverEvent &evt);
	void receive(const MenuShowEvent &evt);

protected:
	CreateGamePlayerType m_playerTypes[GameConstants::MAX_PLAYERS];
	uint8_t m_numPlayers;
	TimeDelta m_resetTime = 0;
	bool m_menuVisible = false;
};

class ServerGame : public LocalGame
{
public:
	bool isServer() override { return true; }
};

class ClientGame : public Game, public Receiver<ClientGame>
{
public:
	ClientGame();
	~ClientGame();

	void receive(const ResetGameEvent &evt);
	void receive(const HoldingStatusEvent &evt);
	bool isClient() override { return true; }
};
