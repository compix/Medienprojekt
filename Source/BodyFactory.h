#pragma once
#include <Box2D/Box2D.h>

class BodyFactory
{
	
public:
	static b2World* m_World;

	enum CollsionCategory {
		NOTHING		= 0x0000,
		PLAYER		= 0x0001,
		SOLID_BLOCK = 0x0002,
		DESTR_BLOCK = 0x0004
	};

	BodyFactory(b2World b2_world);
	~BodyFactory();

	static b2Body* CreateBox(float posX, float posY, float width, float height, b2BodyType type, uint16 isA, uint16 collideWith);
	static b2Body* CreateCircle(float posX, float posY, float radius, b2BodyType type, uint16 isA, uint16 collideWith);
};

