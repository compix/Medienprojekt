#include "BodyFactory.h"
#include "PhysixSystem.h"

b2World* BodyFactory::m_World(new b2World(b2Vec2()));

BodyFactory::BodyFactory(b2World b2_world)
{
}


BodyFactory::~BodyFactory()
{
}

b2Body* BodyFactory::CreateBox(float posX, float posY, float width, float height, b2BodyType type, uint16 isA, uint16 collideWith, bool isSensor)
{
	b2BodyDef bodyDef;
	bodyDef.type = type;
	bodyDef.position.Set(PhysixSystem::toBox2D(posX), PhysixSystem::toBox2D(posY));
	b2Body* body = m_World->CreateBody(&bodyDef);
	b2PolygonShape dynamicBox;
	dynamicBox.SetAsBox(PhysixSystem::toBox2D(width), PhysixSystem::toBox2D(height));
	b2FixtureDef fixtureDef;
	fixtureDef.isSensor = isSensor;
	fixtureDef.filter.categoryBits = isA;
	fixtureDef.filter.maskBits = collideWith;
	fixtureDef.shape = &dynamicBox;
	fixtureDef.density = 1.0f;
	fixtureDef.friction = 0.f;
	body->CreateFixture(&fixtureDef);

	return body;
}

b2Body* BodyFactory::CreateCircle(float posX, float posY, float radius, b2BodyType type, uint16 isA, uint16 collideWith, bool isSensor)
{
	b2BodyDef bodyDef;
	bodyDef.type = type;
	bodyDef.position.Set(PhysixSystem::toBox2D(posX), PhysixSystem::toBox2D(posY));
	b2Body* body = m_World->CreateBody(&bodyDef);
	b2CircleShape dynamicCircle;
	dynamicCircle.m_radius = PhysixSystem::toBox2D(radius);
	b2FixtureDef fixtureDef;
	fixtureDef.isSensor = isSensor;
	fixtureDef.filter.categoryBits = isA;
	fixtureDef.filter.maskBits = collideWith;
	fixtureDef.shape = &dynamicCircle;
	fixtureDef.density = 1.0f;
	fixtureDef.friction = 0.f;
	body->CreateFixture(&fixtureDef);

	return body;
}

