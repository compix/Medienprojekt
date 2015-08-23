#pragma once
#include <Box2D/Box2D.h>
#include <entityx/Entity.h>

using entityx::Entity;

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
		PLAYER		= 0x0010,
		SOLID_BLOCK = 0x0020,
		DESTR_BLOCK = 0x0040,
		BOMB		= 0x0080,
		IS_ON_BOMB	= 0x0100,
		BOMB_RADAR  = 0x0200
	};


	BodyFactory(b2World b2_world);
	~BodyFactory();

	static b2Body* CreateBox(Entity entity, float posX, float posY, float width, float height, b2BodyType type, uint16 isA, uint16 collideWith, bool isSensor = false);
	static b2Body* CreateCircle(Entity entity, float posX, float posY, float radius, b2BodyType type, uint16 isA, uint16 collideWith, bool isSensor = false);
	static b2BodyDef* CreateBodyDef(b2BodyDef* bodyDef, float posX, float posY, b2BodyType type);
	static b2PolygonShape* CreateBoxShape(b2PolygonShape* shape, float width, float height);
	static b2CircleShape* CreateCircleShape(b2CircleShape* shape, float radius);
	static b2FixtureDef* CreateFixtureDef(b2FixtureDef* fixtureDef, b2Shape* shape, uint16 isA, uint16 collideWith, bool isSensor = false);
	static b2Body* CreateBody(b2BodyDef* bodyDef, b2FixtureDef* fixtureDef);

	static bool ShouldCollide(b2Fixture* fixtureA, b2Fixture* fixtureB);
	static bool isA(b2Fixture* fixture, uint16 category);
	static bool contactBetween(b2Contact* contact, uint16 categoryA, uint16 categoryB);
	//static b2Body* BodyFactory::CreateBoxSensor(float posX, float posY, float radius, b2BodyType type, uint16 isA, uint16 collideWith);
};

