#pragma once
#include <entityx/entityx.h>
#include<Box2D/Box2D.h>
#include "Utils/TextureLoader.h"
#include "LayerManager.h"

namespace ExplosionDirection{
	enum Direction;
}

using namespace entityx;

class EntityFactory
{
public:
	EntityFactory(EntityX* entityX, TextureLoader* textureLoader, b2World* world, LayerManager* layerManager);

	// TODO: Clean this up: remove code duplications. Need an easier way to create entities (with a builder).
	
	Entity createTestEntity1(int row, int col);
	Entity createTestEntity2();

	Entity createBlock(int row, int col);
	Entity createSolidBlock(int row, int col);

	Entity createBomb(int row, int col);
	
	Entity createExplosion(int row, int col, ExplosionDirection::Direction direction, int range, float spreadTime, float lifeTime, bool visible);
	Entity createExplosion(int row, int col, int range, float spreadTime);
private:
	EntityX* m_entityX;
	TextureLoader* m_textureLoader;
	b2World* m_world;
	LayerManager* m_layerManager;
};

