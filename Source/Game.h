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

	void refreshView();
	inline const sf::View& getView() const { return m_view; }
protected:
	virtual void addSystems() = 0;

protected:
	sf::View m_view;
	bool initialized = false;
	EntityManager m_entities;
	SystemManager m_systems;
	unique_ptr<EntityFactory> m_entityFactory;
	unique_ptr<LayerManager> m_layerManager;
	PhysixSystem* m_PhysixSystem;
	SFMLDebugDraw m_debugDraw;
	ContactListener listener;
	uint8_t m_width;
	uint8_t m_height;

	Light m_light;
	float m_timer;

	ShaderManager m_shaderManager;

	sf::Vector2f m_mousePos;

	ParticleEmitter* m_particleEmitter;
};

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

protected:
	void addSystems() override;

protected:
	CreateGamePlayerType m_playerTypes[GameConstants::MAX_PLAYERS];
	uint8_t m_numPlayers;
	TimeDelta m_resetTime = 0;
};

class ServerGame : public LocalGame
{
};

class ClientGame : public Game, public Receiver<ClientGame>
{
public:
	ClientGame();
	~ClientGame();

	void receive(const ResetGameEvent &evt);
	bool isClient() override { return true; }

protected:
	void addSystems() override;
};
