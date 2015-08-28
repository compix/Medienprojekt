#pragma once
#include <entityx/System.h>

class ParticleSystem;
class ParticleEmitter;

class ParticleEffects
{
public:
	static void init(ParticleSystem* particleSystem);

	static ParticleEmitter* lightningBomb(uint8_t range);
	static ParticleEmitter* lightningBombPeak();
	static ParticleEmitter* ghostBomb();
	static ParticleEmitter* normalBomb();
	static ParticleEmitter* lava();
	static ParticleEmitter* itemSpawn(entityx::Entity& item);
	static ParticleEmitter* blockDeath();

private:
	static ParticleSystem*  m_particleSystem;
};