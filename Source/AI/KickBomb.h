#pragma once
#include "Behavior.h"

class KickBomb : public Behavior
{
public:
	void update(entityx::Entity& entity) override;
};