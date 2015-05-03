#pragma once
#include <entityx/entityx.h>
#include<Box2D/Box2D.h>
#include "Utils/TextureLoader.h"
#include "PhysixSystem.h"

using namespace entityx;

class EntityFactory
{
public:
	EntityFactory(EntityX* pEntityX, TextureLoader* pTextureLoader, b2World* m_World);

	Entity createTestEntity1();
	Entity createTestEntity2();

	Entity createBlock(float x, float y);
	Entity createSolidBlock(float x, float y);
private:
	EntityX* m_pEntityX;
	TextureLoader* m_pTextureLoader;
	b2World* m_World;
};

