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
	Entity createTestEntity2();

	Entity createBlock(uint8_t row, uint8_t col);
	Entity createSolidBlock(uint8_t row, uint8_t col);

	Entity createBomb(uint8_t row, uint8_t col, Entity owner);
	
	Entity createExplosion(uint8_t row, uint8_t col, Direction direction, uint8_t range, float spreadTime);
	Entity createExplosion(uint8_t row, uint8_t col, uint8_t range, float spreadTime);

	Entity createFloor(uint8_t row, uint8_t col);
private:
	PhysixSystem* m_physixSystem;
	LayerManager* m_layerManager;
	ShaderManager* m_shaderManager;
	entityx::SystemManager* m_systemManager;
};

