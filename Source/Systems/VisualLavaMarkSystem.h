#pragma once
#include <entityx/System.h>

class VisualLavaMarkSystem : public entityx::System<VisualLavaMarkSystem>
{
public:
	void update(entityx::EntityManager& entities, entityx::EventManager& events, entityx::TimeDelta dt) override;
};