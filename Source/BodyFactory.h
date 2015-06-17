#pragma once
#include <Box2D/Box2D.h>

class BodyFactory
{
	
public:
	static b2World* m_World;

	enum CollsionCategory {
		NOTHING		= 0x0000,
		PLAYER_1	= 0x0001,
		PLAYER_2	= 0x0002,
		PLAYER_3	= 0x0004,
		PLAYER_4	= 0x0008,
		SOLID_BLOCK = 0x0010,
		DESTR_BLOCK = 0x0020,
		BOMB		= 0x0040,
		IS_ON_BOMB	= 0x0080,
		BOMB_RADAR  = 0x0100
	};

	BodyFactory(b2World b2_world);
	~BodyFactory();

	static b2Body* CreateBox(float posX, float posY, float width, float height, b2BodyType type, uint16 isA, uint16 collideWith, bool isSensor = false);
	static b2Body* CreateCircle(float posX, float posY, float radius, b2BodyType type, uint16 isA, uint16 collideWith, bool isSensor = false);
	static b2BodyDef* CreateBodyDef(float posX, float posY, b2BodyType type);
	static b2PolygonShape* CreateShape(float width, float height);
	static b2FixtureDef* CreateFixture(b2PolygonShape* shape, uint16 isA, uint16 collideWith, bool isSensor = false);
	//static b2Body* BodyFactory::CreateBoxSensor(float posX, float posY, float radius, b2BodyType type, uint16 isA, uint16 collideWith);
};

