#pragma once
#include "Behavior.h"

class UseSkill : public Behavior
{
public:
	void operator()(entityx::Entity& entity);
};