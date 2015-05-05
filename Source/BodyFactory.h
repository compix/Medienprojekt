#pragma once
#include <Box2D/Box2D.h>

class BodyFactory
{
	
public:
	static b2World* m_World;

	enum CollsionCategory {
		PLAYER = 0x0001,
		WALL = 0x0002
	};

	BodyFactory(b2World b2_world);
	~BodyFactory();

	static b2Body* CreateBox(float posX, float posY, float width, float height, b2BodyType type, uint16 isA, uint16 collideWith);
};

