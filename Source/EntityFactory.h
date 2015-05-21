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
	EntityFactory(EntityManager &entities, TextureLoader* textureLoader, PhysixSystem* physixSystem, LayerManager* layerManager, ShaderManager* shaderManager);

	// TODO: Clean this up: remove code duplications. Need an easier way to create entities (with a builder).
	
	Entity createTestEntity1(int row, int col);
	Entity createTestEntity2();

	Entity createBlock(int row, int col);
	Entity createSolidBlock(int row, int col);

	Entity createBomb(int row, int col, Entity owner);
	
	Entity createExplosion(int row, int col, Direction direction, int range, float spreadTime, bool visible);
	Entity createExplosion(int row, int col, int range, float spreadTime);

	Entity createFloor(int row, int col);
private:
	EntityManager &m_entities;
	TextureLoader* m_textureLoader;
	PhysixSystem* m_PhysixSystem;
	LayerManager* m_layerManager;
	ShaderManager* m_shaderManager;
};

