#pragma once
#include <Box2D/Dynamics/b2Body.h>
#include "../../Source/PhysixSystem.h"

class BodyBuilder
{
	static b2World* m_World;

	enum CollsionCategory {
		NOTHING = 0x0000,
		PLAYER = 0x0001,
		SOLID_BLOCK = 0x0002,
		DESTR_BLOCK = 0x0004,
		BOMB = 0x0008,
		ON_BOMB = 0x0010
	};

private:
	float x, y, width, height, denisty, friction;
	b2BodyType type;
	uint16 is_A, collide_With;
	bool isSensor;

	bool b_Type, b_Pos, b_Box, b_Sens, b_Cat, b_Coll, b_Dens, b_Fric;

public:
	b2Body* build();
	void setType(b2BodyType type);
	void setPosition(float x, float y);
	void setBox(float width, float height);
	void setSensor(bool isSensor);
	void isA(uint16 is_A);
	void collideWith(uint16 collide_With);
	void setDensity(float denisty);
	void setFriction(float friction);
};
