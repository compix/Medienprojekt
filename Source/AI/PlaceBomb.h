#pragma once
#include "Behavior.h"

class PlaceBomb : public Behavior
{
public:
	void update(entityx::Entity& entity) override;
};