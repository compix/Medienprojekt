#include "BodyBuilder.h"


b2Body* BodyBuilder::build()
{
	b2BodyDef bodyDef;
	if (b_Type){
		bodyDef.type = type;
	}
	if (b_Pos){
		bodyDef.position.Set(PhysixSystem::toBox2D(x), PhysixSystem::toBox2D(y));
	}
	b2Body* body = m_World->CreateBody(&bodyDef);
	b2PolygonShape shape;
	if (b_Box){
		shape.SetAsBox(PhysixSystem::toBox2D(width), PhysixSystem::toBox2D(height));
	}
	b2FixtureDef fixtureDef;
	if (b_Sens){
		fixtureDef.isSensor = isSensor;
	}
	/*if (b_Cat){
		fixtureDef.filter.categoryBits = isA;
	}
	if (b_Coll){
		fixtureDef.filter.maskBits = collideWith;
	}*/
	fixtureDef.shape = &shape;
	if (b_Dens){
		fixtureDef.density = 1.0f;
	}
	if (b_Fric){
		fixtureDef.friction = 0.f;
	}
	body->CreateFixture(&fixtureDef);

	return body;
}

void BodyBuilder::setType(b2BodyType type)
{
	this->type = type;
}

void BodyBuilder::setPosition(float x, float y)
{
	this->x = x;
	this->y = y;
}

void BodyBuilder::setBox(float width, float height)
{
	this->width = width;
	this->height = height;
}

void BodyBuilder::setSensor(bool isSensor)
{
	
}