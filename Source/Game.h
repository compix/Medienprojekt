#pragma once
#include "entityx/entityx.h"
#include <SFML/Graphics.hpp>
#include <memory>
#include "Utils/TextureLoader.h"
#include "EntityFactory.h"
#include "LayerManager.h"

using entityx::EntityX;
using entityx::TimeDelta;
using std::unique_ptr;

class InputManager;

class Game : public EntityX
{
public:
	Game(sf::RenderWindow* window, InputManager &inputManager);
	~Game();

	void update(TimeDelta dt);

private:
	void createTestLevel(EntityLayer& layer);

	void testExplosions(TimeDelta dt);
private:
	unique_ptr<TextureLoader> m_textureLoader;
	unique_ptr<EntityFactory> m_entityFactory;
	unique_ptr<LayerManager> m_layerManager;
	unique_ptr<b2World> m_world;

	float m_timer;
};

