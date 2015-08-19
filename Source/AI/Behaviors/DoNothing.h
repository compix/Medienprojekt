#pragma once
#include <entityx/Entity.h>

class DoNothing
{
public:
	void operator()(entityx::Entity& entity) {}
};