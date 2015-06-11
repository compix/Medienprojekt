#pragma once
#include <entityx/System.h>


using entityx::System;
using entityx::EntityManager;
using entityx::EventManager;
using entityx::TimeDelta;

class SoundSystem : public entityx::System<SoundSystem>
{
public:
	SoundSystem();
	void update(EntityManager &entityManager, EventManager &eventManager, TimeDelta dt) override;
	~SoundSystem();
};

