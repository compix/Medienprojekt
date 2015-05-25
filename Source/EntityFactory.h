#pragma once
#include <entityx/entityx.h>
#include<Box2D/Box2D.h>
#include "Utils/TextureLoader.h"
#include "PhysixSystem.h"
#include "LayerManager.h"
#include "Utils/Common.h"
#include "Components/ExplosionComponent.h"

class ShaderManager;
using entityx::EntityManager;

class EntityFactory
{
public:
	EntityFactory(PhysixSystem* physixSystem, LayerManager* layerManager, ShaderManager* shaderManager, entityx::SystemManager* systemManager);

	// TODO: Clean this up: remove code duplications. Need an easier way to create entities (with a builder).

	Entity createPlayer(float x, float y);
	Entity createTestEntity1(int row, int col);
	Entity createTestEntity2();

	Entity createBlock(int row, int col);
	Entity createSolidBlock(int row, int col);

	Entity createBomb(int row, int col, Entity owner);
	
	Entity createExplosion(int row, int col, Direction direction, int range, float spreadTime);
	Entity createExplosion(int row, int col, int range, float spreadTime);

	Entity createFloor(int row, int col);
private:
	PhysixSystem* m_physixSystem;
	LayerManager* m_layerManager;
	ShaderManager* m_shaderManager;
	int m_playerIndex = 0;
	entityx::SystemManager* m_systemManager;
};

