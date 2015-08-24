#pragma once
#include <entityx/Entity.h>

class DoNothing
{
public:
	bool operator()(entityx::Entity& entity) { return true; }
};