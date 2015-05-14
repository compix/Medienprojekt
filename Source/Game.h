#pragma once
#include "entityx/entityx.h"
#include <SFML/Graphics.hpp>
#include <memory>
#include "Utils/TextureLoader.h"
#include "EntityFactory.h"
#include "LayerManager.h"
#include "SFMLDebugDraw.h"
#include "Lighting/Light.h"
#include "Utils/Shaders.h"

using entityx::TimeDelta;
using entityx::EventManager;
using entityx::SystemManager;
using std::unique_ptr;

class InputManager;

class Game
{
public:
	Game(sf::RenderWindow* window, InputManager &inputManager, EventManager &events, SFMLDebugDraw* debugDraw);
	~Game();

	void update(TimeDelta dt);

	inline void setMousePos(sf::Vector2f mousePos) { m_mousePos = mousePos; }
private:
	void testExplosions(TimeDelta dt);

private:
	EventManager &m_events;
	EntityManager m_entities;
	SystemManager m_systems;
	unique_ptr<TextureLoader> m_textureLoader;
	unique_ptr<EntityFactory> m_entityFactory;
	unique_ptr<LayerManager> m_layerManager;
	PhysixSystem* m_PhysixSystem;
	SFMLDebugDraw* debug;

	sf::RenderWindow* m_window;
	Light m_light;
	float m_timer;

	ShaderManager m_shaderManager;

	sf::Vector2f m_mousePos;
};

