#pragma once
#include <entityx/Entity.h>

class UseDirectionSkill
{
public:
	bool operator()(entityx::Entity& entity);
};