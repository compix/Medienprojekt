﻿#pragma once
#include "entityx/entityx.h"
#include <SFML/Graphics.hpp>
#include <memory>
#include "Utils/TextureLoader.h"
#include "EntityFactory.h"
#include "LayerManager.h"
#include "SFMLDebugDraw.h"
#include "Lighting/Light.h"
#include "Utils/ShaderManager.h"
#include "Graphics/ParticleEmitter.h"

using entityx::TimeDelta;
using entityx::EventManager;
using entityx::SystemManager;
using std::unique_ptr;

class Game
{
public:
	Game();
	~Game();

	void update(TimeDelta dt);

	inline void setMousePos(sf::Vector2f mousePos) { m_mousePos = mousePos; }

private:
	EntityManager m_entities;
	SystemManager m_systems;
	unique_ptr<EntityFactory> m_entityFactory;
	unique_ptr<LayerManager> m_layerManager;
	PhysixSystem* m_PhysixSystem;
	SFMLDebugDraw m_debugDraw;

	Light m_light;
	float m_timer;

	ShaderManager m_shaderManager;

	sf::Vector2f m_mousePos;

	ParticleEmitter m_particleEmitter;
};

