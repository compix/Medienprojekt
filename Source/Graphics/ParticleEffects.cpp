#include "ParticleEffects.h"
#include "../Systems/ParticleSystem.h"
#include "../Utils/Functions.h"
#include "ParticleEmitter.h"
#include "../SFMLDebugDraw.h"

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
		.spawnWidth(GameConstants::CELL_WIDTH - 5.f)
		.spawnHeight(float(GameConstants::CELL_HEIGHT))
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
		.spawnWidth(GameConstants::CELL_WIDTH - 5.f)
		.spawnHeight(float(GameConstants::CELL_HEIGHT))
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
		.spawnWidth(GameConstants::CELL_WIDTH - 5.f)
		.spawnHeight(float(GameConstants::CELL_HEIGHT))
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
		.spawnWidth(GameConstants::CELL_WIDTH - 5.f)
		.spawnHeight(float(GameConstants::CELL_HEIGHT))
		.spawnDuration(0.3f)
		.colorFunction(Gradient<RGB>(GradientType::REGRESS, RGB(5, 42, 252), RGB(255, 102, 0)));

	return emitter;
}

ParticleEmitter* ParticleEffects::lava()
{
	assert(m_particleSystem);

	auto manager = m_particleSystem->getManager("lava");
	auto emitter = manager->spawnEmitter();

	if (!emitter)
		return nullptr;

	emitter->spawnTime(0.015f)
		.maxLifetime(1.f)
		.gravityModifier(-1.f)
		.speedModifier(0.1f)
		.velocityFunction([](float t) { return sf::Vector2f(sin(t), cos(t) * 25.f); })
		//.angularVelocityFunction(Gradient<float>(GradientType::SMOOTH, 0, Math::PI*0.05f))
		.sizeFunction(Gradient<sf::Vector2f>(GradientType::LINEAR, sf::Vector2f(25, 25), sf::Vector2f(30, 30)))
		.spawnWidth(GameConstants::CELL_WIDTH - 5.f)
		.spawnHeight(float(GameConstants::CELL_HEIGHT))
		.transparencyFunction(Gradient<float>(GradientType::SMOOTH, 150, 0))
		.colorFunction(Gradient<RGB>(GradientType::REGRESS, RGB(5, 42, 252), RGB(255, 102, 0)));

	return emitter;
}

ParticleEmitter* ParticleEffects::itemSpawn()
{
	assert(m_particleSystem);

	auto manager = m_particleSystem->getManager("light");
	auto emitter = manager->spawnEmitter();

	if (!emitter)
		return nullptr;

	emitter->spawnTime(0.015f)
		.maxLifetime(1.f)
		.gravityModifier(1.f)
		.speedModifier(20.f)
		.velocityFunction([](float t) { return sf::Vector2f(sin(t), cos(t)); })
		.angularVelocityFunction(Gradient<float>(GradientType::SMOOTH, 0, Math::PI*0.05f))
		.sizeFunction(Gradient<sf::Vector2f>(GradientType::LINEAR, sf::Vector2f(25, 25), sf::Vector2f(30, 30)))
		.spawnWidth(GameConstants::CELL_WIDTH + 20.f)
		.spawnHeight(GameConstants::CELL_HEIGHT + 20.f)
		.spawnDuration(0.5f)
		.colorFunction(Gradient<RGB>(GradientType::REGRESS, RGB(255, 255, 255), RGB(255, 252, 0)));

	return emitter;
}

ParticleEmitter* ParticleEffects::blockDeath()
{
	assert(m_particleSystem);

	auto manager = m_particleSystem->getManager("block");
	auto emitter = manager->spawnEmitter();

	if (!emitter)
		return nullptr;

	emitter->spawnTime(10.f)
		.spawnDuration(1.0f)
		.maxLifetime(0.9f)
		.speedModifier(10.f)
		.burstParticleNumber(150)
		.velocityFunction(Gradient<sf::Vector2f>(GradientType::SMOOTH, sf::Vector2f(1, 1), sf::Vector2f(5, 5)))
		.burstTime(0.01f)
		.burstNumber(1)
		.spawnWidth(15.f)
		.spawnHeight(15.f)
		.angularVelocityFunction(Gradient<float>(GradientType::SMOOTH, 0, Math::PI*0.05f))
		.sizeFunction(Gradient3<sf::Vector2f>(GradientType::REGRESS, sf::Vector2f(60, 60), sf::Vector2f(20, 20), sf::Vector2f(5, 5)));

	return emitter;
}