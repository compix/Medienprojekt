#pragma once
#include <ecstasy/core/Component.h>
#include "../Graphics/ParticleEmitter.h"

struct ParticleComponent: public Component<ParticleComponent>
{
	ParticleComponent(ParticleEmitter* emitter) : emitter(emitter) {}

	ParticleEmitter* emitter;
};