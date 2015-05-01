#pragma once
#include <entityx/entityx.h>
#include<Box2D/Box2D.h>
#include "Utils/TextureLoader.h"
#include "LayerManager.h"

using namespace entityx;

class EntityFactory
{
public:
	EntityFactory(EntityX* entityX, TextureLoader* textureLoader, b2World* world, LayerManager* layerManager);

	Entity createTestEntity1(int row, int col);
	Entity createTestEntity2();

	Entity createBlock(int row, int col);
	Entity createSolidBlock(int row, int col);
private:
	EntityX* m_entityX;
	TextureLoader* m_textureLoader;
	b2World* m_world;
	LayerManager* m_layerManager;
};

