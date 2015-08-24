#pragma once
#include <entityx/Entity.h>

class PunchBomb
{
public:
	bool operator()(entityx::Entity& entity);
};