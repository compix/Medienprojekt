#pragma once
#include "../Graphics/ParticleEmitter.h"

struct ParticleComponent
{
	ParticleComponent() {}
	ParticleComponent(ParticleEmitter* emitter) : emitter(emitter) {}

	ParticleEmitter* emitter;
};