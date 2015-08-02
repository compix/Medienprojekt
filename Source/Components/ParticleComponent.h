#pragma once
#include <ecstasy/core/Component.h>
#include "../Graphics/ParticleEmitter.h"

struct ParticleComponent: public ECS::Component<ParticleComponent>
{
	ParticleEmitter* emitter = nullptr;
	
	void reset() override {
		emitter = nullptr;
	}
};