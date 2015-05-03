#include "Game.h"
#include "Systems/AnimationSystem.h"
#include "Systems/InputSystem.h"
#include "Systems/BodySystem.h"
#include "Systems/RenderSystem.h"
#include "Components/TransformComponent.h"
#include <Box2D/Box2D.h>


#ifndef _MSC_VER

template<typename T, typename... Args>
std::unique_ptr<T> make_unique(Args&&... args)
{
	return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}
#endif


Game::Game(sf::RenderWindow* pWindow, InputManager &inputManager, SFMLDebugDraw* debugDraw)
{


	m_PhysixSystem = new PhysixSystem(6, 3, 1.f);
	m_PhysixSystem->SetDebugDrawer(debugDraw);

	m_pLayerManager = make_unique<LayerManager>();

	systems.add<BodySystem>();
	systems.add<InputSystem>(inputManager);
	systems.add<AnimationSystem>();
	systems.add<RenderSystem>(pWindow, m_pLayerManager.get());
	systems.configure();

	m_pTextureLoader = make_unique<TextureLoader>();
	m_pTextureLoader->loadAllFromJson("assets/json/textures.json");

	m_pEntityFactory = make_unique<EntityFactory>(this, m_pTextureLoader.get(), m_PhysixSystem);

	Entity entity = m_pEntityFactory->createTestEntity1();
	Entity entity2 = m_pEntityFactory->createTestEntity2();

	ComponentHandle<TransformComponent> t = entity.component<TransformComponent>();

	EntityLayer& entityLayer = m_pLayerManager->createLayer(0, false);
	entityLayer.add(entity2);
	entityLayer.add(entity);

	createTestLevel(entityLayer);

	m_pLayerManager->sortLayers(DepthComparator());
}

Game::~Game() { 
	delete m_PhysixSystem;
}

void Game::update(TimeDelta dt)
{
	m_PhysixSystem->Update(dt);
	systems.update_all(dt);
	m_PhysixSystem->DrawDebug();
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

			if (x == 0 || x == 20 || y == 0 || y == 20 || (x % 2 == 0 && y % 2 == 0))
			{
				e = m_pEntityFactory->createSolidBlock(x*blockWidth, y * blockHeight);
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
