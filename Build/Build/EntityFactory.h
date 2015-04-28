#pragma once
#include <entityx/entityx.h>
#include "TextureLoader.h"

using namespace entityx;

class EntityFactory
{
public:
	EntityFactory(EntityX* pEntityX, TextureLoader* pTextureLoader);

	Entity createTestEntity1();
	Entity createTestEntity2();

	Entity createBlock(float x, float y);
	Entity createSolidBlock(float x, float y);
private:
	EntityX* m_pEntityX;
	TextureLoader* m_pTextureLoader;
};

