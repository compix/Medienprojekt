#pragma once
#include "Behavior.h"

class UseSkill : public Behavior
{
public:
	void update(entityx::Entity& entity) override;
};