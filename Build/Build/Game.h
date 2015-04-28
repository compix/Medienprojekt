#pragma once
#include "entityx/entityx.h"
#include <SFML/Graphics.hpp>
#include <memory>
#include "TextureLoader.h"
#include "EntityFactory.h"
#include "LayerManager.h"

using namespace entityx;
using namespace std;

class Game : public EntityX
{
public:
	Game(sf::RenderWindow* pWindow);
	~Game();

	void update(TimeDelta dt);

private:
	void createTestLevel(EntityLayer& layer);

private:
	unique_ptr<TextureLoader> m_pTextureLoader;
	unique_ptr<EntityFactory> m_pEntityFactory;
	unique_ptr<LayerManager> m_pLayerManager;
	unique_ptr<b2World> m_pWorld;
};

