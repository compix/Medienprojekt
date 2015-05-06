﻿#pragma once
#include "entityx/entityx.h"
#include <SFML/Graphics.hpp>
#include <memory>
#include "Utils/TextureLoader.h"
#include "EntityFactory.h"
#include "LayerManager.h"
#include "SFMLDebugDraw.h"

using entityx::EntityX;
using entityx::TimeDelta;
using std::unique_ptr;

class InputManager;

class Game : public EntityX
{
public:
	Game(sf::RenderWindow* window, InputManager &inputManager, SFMLDebugDraw* debugDraw);
	~Game();

	void update(TimeDelta dt);

private:
	void createTestLevel(EntityLayer& layer);

	void testExplosions(TimeDelta dt);
private:
	unique_ptr<TextureLoader> m_textureLoader;
	unique_ptr<EntityFactory> m_entityFactory;
	unique_ptr<LayerManager> m_layerManager;
	PhysixSystem* m_PhysixSystem;
	SFMLDebugDraw* debug;

	float m_timer;
};

