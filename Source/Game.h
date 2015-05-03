#pragma once
#include "entityx/entityx.h"
#include <SFML/Graphics.hpp>
#include <memory>
#include "Utils/TextureLoader.h"
#include "EntityFactory.h"
#include "LayerManager.h"
#include "SFMLDebugDraw.h"

using namespace entityx;
using namespace std;

class InputManager;

class Game : public EntityX
{
public:
	Game(sf::RenderWindow* pWindow, InputManager &inputManager, SFMLDebugDraw* debugDraw);
	~Game();

	void update(TimeDelta dt);

private:
	void createTestLevel(EntityLayer& layer);

private:
	unique_ptr<TextureLoader> m_pTextureLoader;
	unique_ptr<EntityFactory> m_pEntityFactory;
	unique_ptr<LayerManager> m_pLayerManager;
	PhysixSystem* m_PhysixSystem;
	b2World* m_World;
	SFMLDebugDraw* debug;
};

