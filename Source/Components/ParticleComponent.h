#pragma once
#include <Graphics/ParticleEmitter.h>

struct ParticleComponent
{
	ParticleComponent() {}
	ParticleEmitter* emitter;
};