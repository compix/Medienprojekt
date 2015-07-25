#pragma once
#include "Behavior.h"

class Flee : public Behavior
{
public:
	Flee(std::vector<entityx::Entity>& entitiesToAvoid);

	void update(entityx::Entity& entity) override;

	void prepare(entityx::Entity& entity) override;
private:
	std::vector<entityx::Entity> m_entitiesToAvoid;
};