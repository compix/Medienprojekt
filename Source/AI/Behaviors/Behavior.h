#pragma once
#include <../entityx/entityx/entityx.h>

class Behavior
{
public:
	virtual ~Behavior() {}

	virtual void prepare(entityx::Entity& entity) {}
	virtual void update(entityx::Entity& entity) = 0;
};