#pragma once
#include <entityx/Entity.h>

class UseSkill
{
public:
	bool operator()(entityx::Entity& entity);
};