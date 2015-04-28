#include "Game.h"
#include "AnimationSystem.h"
#include "BodySystem.h"
#include "RenderSystem.h"
#include "TransformComponent.h"
#include <Box2D\Box2D.h>

Game::Game(sf::RenderWindow* pWindow)
{

	b2Vec2 gravity(0.0f, 10.0f);
	m_pWorld = make_unique<b2World>(gravity);

	m_pLayerManager = make_unique<LayerManager>();

	systems.add<BodySystem>();
	systems.add<AnimationSystem>();
	systems.add<RenderSystem>(pWindow, m_pLayerManager.get());
	systems.configure();

	m_pTextureLoader = make_unique<TextureLoader>();
	m_pTextureLoader->loadAllFromJson("assets/json/textures.json");

	m_pEntityFactory = make_unique<EntityFactory>(this, m_pTextureLoader.get(), m_pWorld.get());

	Entity entity = m_pEntityFactory->createTestEntity1();
	Entity entity2 = m_pEntityFactory->createTestEntity2();

	ComponentHandle<TransformComponent> t = entity.component<TransformComponent>();

	EntityLayer& entityLayer = m_pLayerManager->createLayer(0, false);
	entityLayer.add(entity2);
	entityLayer.add(entity);

	createTestLevel(entityLayer);

	m_pLayerManager->sortLayers(DepthComparator());
}

Game::~Game()
{
}

void Game::update(TimeDelta dt)
{
	m_pWorld->Step(dt, 6, 2);
	systems.update_all(dt);
}

void Game::createTestLevel(EntityLayer& layer)
{
	int blockWidth = 25;
	int blockHeight = 25;

	for (int x = 0; x < 21; x++)
	{
		for (int y = 0; y < 21; y++)
		{
			Entity e;

			if (x == 0 || x == 20 || y == 0 || y == 20|| (x % 2 == 0 && y % 2 == 0))
			{
				e = m_pEntityFactory->createSolidBlock(x*blockWidth, y*blockHeight);
				layer.add(e);
			}
			else
			{
				//e = m_pEntityFactory->createBlock(x*blockWidth, y*blockHeight);
				//layer.add(e);
			}	
		}
	}
}
