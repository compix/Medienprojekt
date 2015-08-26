#include "ParticleEffects.h"
#include "../Systems/ParticleSystem.h"
#include "../Utils/Functions.h"
#include "ParticleEmitter.h"

ParticleSystem*  ParticleEffects::m_particleSystem = nullptr;

void ParticleEffects::init(ParticleSystem* particleSystem)
{
	m_particleSystem = particleSystem;
}

ParticleEmitter* ParticleEffects::lightningBomb(uint8_t range)
{
	assert(m_particleSystem);

	auto manager = m_particleSystem->getManager("light");
	auto emitter = manager->spawnEmitter();

	if (!emitter)
		return nullptr;

	range = std::max(uint8_t(1), range);

	emitter->spawnTime(0.0015f)
		.maxLifetime(0.3f)
		.gravityModifier(1.f)
		.velocityFunction([](float t) { return sf::Vector2f(t, t*t*t*100.f); })
		.angularVelocityFunction(Gradient<float>(GradientType::SMOOTH, 0, Math::PI*0.05f))
		.sizeFunction(Gradient<sf::Vector2f>(GradientType::LINEAR, sf::Vector2f(20, 20), sf::Vector2f(5, 8)))
		.burstParticleNumber(10)
		.burstTime(0.5f)
		.spawnWidth(50 - 5)
		.spawnHeight(50)
		.spawnDuration(0.3f)
		.colorFunction(Gradient3<RGB>(GradientType::LINEAR, RGB(37, 8, 255.f / range), RGB(0, 252.f / range, 255), RGB(0, 84, 255)));

	return emitter;
}

ParticleEmitter* ParticleEffects::lightningBombPeak()
{
	assert(m_particleSystem);

	auto manager = m_particleSystem->getManager("light");
	auto emitter = manager->spawnEmitter();

	if (!emitter)
		return nullptr;

	emitter->spawnTime(0.0015f)
		.maxLifetime(0.3f)
		.gravityModifier(1.f)
		.velocityFunction([](float t) { return sf::Vector2f(t, t*t*t*110.f); })
		.angularVelocityFunction(Gradient<float>(GradientType::SMOOTH, 0, Math::PI*0.05f))
		.sizeFunction(Gradient<sf::Vector2f>(GradientType::LINEAR, sf::Vector2f(20, 20), sf::Vector2f(5, 8)))
		.burstParticleNumber(10)
		.burstTime(0.5f)
		.spawnWidth(50 - 5)
		.spawnHeight(50)
		.spawnDuration(0.3f)
		.colorFunction(Gradient<RGB>(GradientType::REGRESS, RGB(0, 252, 255), RGB(163, 8, 173)));

	return emitter;
}

ParticleEmitter* ParticleEffects::ghostBomb()
{
	assert(m_particleSystem);

	auto manager = m_particleSystem->getManager("light");
	auto emitter = manager->spawnEmitter();

	if (!emitter)
		return nullptr;

	emitter->spawnTime(0.0015f)
		.maxLifetime(0.3f)
		.gravityModifier(1.f)
		.velocityFunction([](float t) { return sf::Vector2f(sinf(t) * t * 100.f, cosf(t)*t*t*100.f); })
		.angularVelocityFunction(Gradient<float>(GradientType::SMOOTH, 0, Math::PI*0.05f))
		.sizeFunction(Gradient<sf::Vector2f>(GradientType::LINEAR, sf::Vector2f(15, 15), sf::Vector2f(20, 20)))
		.burstParticleNumber(10)
		.burstTime(0.5f)
		.spawnWidth(50 - 5)
		.spawnHeight(50)
		.spawnDuration(0.3f)
		.colorFunction(Gradient<RGB>(GradientType::REGRESS, RGB(255, 0, 0), RGB(0, 255, 12)));

	return emitter;
}

ParticleEmitter* ParticleEffects::normalBomb()
{
	assert(m_particleSystem);

	auto manager = m_particleSystem->getManager("light");
	auto emitter = manager->spawnEmitter();

	if (!emitter)
		return nullptr;

	emitter->spawnTime(0.0015f)
		.maxLifetime(0.3f)
		.gravityModifier(1.f)
		.velocityFunction([](float t) { return sf::Vector2f(t, t*t*t*100.f); })
		.angularVelocityFunction(Gradient<float>(GradientType::SMOOTH, 0, Math::PI*0.05f))
		.sizeFunction(Gradient<sf::Vector2f>(GradientType::LINEAR, sf::Vector2f(15, 15), sf::Vector2f(20, 20)))
		.burstParticleNumber(10)
		.burstTime(0.5f)
		.spawnWidth(50 - 5)
		.spawnHeight(50)
		.spawnDuration(0.3f)
		.colorFunction(Gradient<RGB>(GradientType::REGRESS, RGB(5, 42, 252), RGB(255, 102, 0)));

	return emitter;
}