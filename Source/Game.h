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

using entityx::TimeDelta;
using entityx::EventManager;
using entityx::SystemManager;
using std::unique_ptr;

class ParticleEmitter;

class Game
{
public:
	Game();
	virtual ~Game();
	virtual void init(uint8_t width, uint8_t height);

	void update(TimeDelta dt);

	inline void setMousePos(sf::Vector2f mousePos) { m_mousePos = mousePos; }

	uint8_t getWidth() { return m_width; }
	uint8_t getHeight() { return m_height; }
protected:
	virtual void addSystems() = 0;

protected:
	bool initialized = false;
	EntityManager m_entities;
	SystemManager m_systems;
	unique_ptr<EntityFactory> m_entityFactory;
	unique_ptr<LayerManager> m_layerManager;
	PhysixSystem* m_PhysixSystem;
	SFMLDebugDraw m_debugDraw;
	uint8_t m_width;
	uint8_t m_height;

	Light m_light;
	float m_timer;

	ShaderManager m_shaderManager;

	sf::Vector2f m_mousePos;

	ParticleEmitter* m_particleEmitter;
};

class LocalGame : public Game
{
public:
	void init(uint8_t width, uint8_t height) override;

protected:
	void addSystems() override;
};

class ServerGame : public LocalGame
{
};

class ClientGame : public Game
{
protected:
	void addSystems() override;
};
