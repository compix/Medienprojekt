#pragma once
#include <entityx/entityx.h>
#include<Box2D/Box2D.h>
#include "Utils/TextureLoader.h"
#include "PhysixSystem.h"
#include "LayerManager.h"

using entityx::EntityX;

class EntityFactory
{
public:
	EntityFactory(EntityX* entityX, TextureLoader* textureLoader, PhysixSystem* physixSystem, LayerManager* layerManager);

	Entity createTestEntity1(int row, int col);
	Entity createTestEntity2();

	Entity createBlock(int row, int col);
	Entity createSolidBlock(int row, int col);
private:
	EntityX* m_entityX;
	TextureLoader* m_textureLoader;
	PhysixSystem* m_PhysixSystem;
	LayerManager* m_layerManager;
};

