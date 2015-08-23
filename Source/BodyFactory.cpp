#include "BodyFactory.h"
#include "PhysixSystem.h"


b2World* BodyFactory::m_World(new b2World(b2Vec2()));

BodyFactory::BodyFactory(b2World b2_world)
{
	m_World = &b2_world;
}


BodyFactory::~BodyFactory()
{
}

b2Body* BodyFactory::CreateBox(Entity entity, float posX, float posY, float width, float height, b2BodyType type, uint16 isA, uint16 collideWith, bool isSensor)
{

	b2PolygonShape shape;
	CreateBoxShape(&shape, width, height);
	b2FixtureDef fixtureDef;
	b2BodyDef bodyDef;  

	b2Body* body = CreateBody(CreateBodyDef(&bodyDef, posX, posY, type), 
							  CreateFixtureDef(&fixtureDef, &shape, isA, collideWith, isSensor));
	int userData = entity.id().id();
	body->SetUserData(reinterpret_cast<void*>(userData));
	return body;
}

b2Body* BodyFactory::CreateCircle(Entity entity, float posX, float posY, float radius, b2BodyType type, uint16 isA, uint16 collideWith, bool isSensor)
{
	b2CircleShape shape;
	CreateCircleShape(&shape,radius);
	b2FixtureDef fixtureDef;
	b2BodyDef bodyDef;

	b2Body* body = CreateBody(CreateBodyDef(&bodyDef, posX, posY, type),
							  CreateFixtureDef(&fixtureDef, &shape, isA, collideWith, isSensor));
	int userData = entity.id().id();
	body->SetUserData(reinterpret_cast<void*>(userData));
	body->GetUserData();
	return body;
}

b2BodyDef* BodyFactory::CreateBodyDef(b2BodyDef* bodyDef, float posX, float posY, b2BodyType type)
{
	bodyDef->type = type;
	bodyDef->position.Set(PhysixSystem::toBox2D(posX), PhysixSystem::toBox2D(posY));
	return bodyDef;
}

b2PolygonShape* BodyFactory::CreateBoxShape(b2PolygonShape* shape, float width, float height)
{
	shape->SetAsBox(PhysixSystem::toBox2D(width), PhysixSystem::toBox2D(height));
	return shape;
}

b2CircleShape* BodyFactory::CreateCircleShape(b2CircleShape* shape, float radius)
{
	shape->m_radius = PhysixSystem::toBox2D(radius);
	return shape;
}

b2FixtureDef* BodyFactory::CreateFixtureDef(b2FixtureDef* fixtureDef, b2Shape* shape, uint16 isA, uint16 collideWith, bool isSensor)
{
	fixtureDef->isSensor = isSensor;
	fixtureDef->filter.categoryBits = isA;
	fixtureDef->filter.maskBits = collideWith;
	fixtureDef->shape = shape;
	
	fixtureDef->density = 1.0f;
	fixtureDef->friction = 0.f;
	return fixtureDef;
}

b2Body* BodyFactory::CreateBody(b2BodyDef* bodyDef, b2FixtureDef* fixtureDef)
{
	b2Body* body = m_World->CreateBody(bodyDef);
	body->CreateFixture(fixtureDef);
	return body;
}


bool BodyFactory::ShouldCollide(b2Fixture* fixtureA, b2Fixture* fixtureB)
{
	return ((fixtureA->GetFilterData().categoryBits & fixtureB->GetFilterData().maskBits) != 0);
}

bool BodyFactory::isA(b2Fixture* fixture, uint16 category)
{
	return ((fixture->GetFilterData().categoryBits & category) != 0);
}

bool BodyFactory::contactBetween(b2Contact* contact, uint16 categoryA, uint16 categoryB){
	b2Fixture* fixtureA = contact->GetFixtureA();
	b2Fixture* fixtureB = contact->GetFixtureB();
	if ((isA(fixtureA, categoryA) || isA(fixtureA, categoryB)) && (isA(fixtureB, categoryA) || isA(fixtureB, categoryB)))
	{
		return true;
	}
	return false;
}

