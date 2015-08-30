#pragma once
#include <entityx/Entity.h>

/**
* Some actions don't require an explicit behavior. Following the path is enough.
*/
class DoNothing
{
public:
	bool operator()(entityx::Entity& entity) { return true; }
};