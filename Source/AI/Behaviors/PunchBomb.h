#pragma once
#include <entityx/Entity.h>

class PunchBomb
{
public:
	void operator()(entityx::Entity& entity);
};