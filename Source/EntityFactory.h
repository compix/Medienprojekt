#pragma once
#include <entityx/entityx.h>
#include<Box2D/Box2D.h>
#include "Utils/TextureLoader.h"
#include "PhysixSystem.h"
#include "LayerManager.h"
#include "Utils/Common.h"

using entityx::EntityX;

class EntityFactory
{
public:
	EntityFactory(EntityX* entityX, TextureLoader* textureLoader, PhysixSystem* physixSystem, LayerManager* layerManager);

	// TODO: Clean this up: remove code duplications. Need an easier way to create entities (with a builder).
	
	Entity createTestEntity1(int row, int col);
	Entity createTestEntity2();

	Entity createBlock(int row, int col);
	Entity createSolidBlock(int row, int col);

	Entity createBomb(int row, int col);
	
	Entity createExplosion(int row, int col, Common::Direction direction, int range, float spreadTime, bool visible);
	Entity createExplosion(int row, int col, int range, float spreadTime);
private:
	EntityX* m_entityX;
	TextureLoader* m_textureLoader;
	PhysixSystem* m_PhysixSystem;
	LayerManager* m_layerManager;
};

