#pragma once
#include "Particle.h"
#include <vector>
#include <SFML/Graphics.hpp>
#include <functional>

const float GRAVITY = 9.81f;

typedef std::function < float(float) > MathFunction; // returns f(time)
typedef std::function < sf::Vector2f(float) > Vec2Function; // returns 2d vector f(time)
typedef std::function < sf::Vector3f(float) > Vec3Function; // returns 3d vector f(time)
typedef std::function < sf::Color(float) > ColorFunction; // returns 3d vector f(time)

/**
 * @brief	Functions are used to manipulate particles: Input is the time in range ~[-1, 1]. Output depends on the function.
 */
class ParticleEmitter : public sf::Drawable, public sf::Transformable
{
public:
	ParticleEmitter();
	ParticleEmitter(int maxParticles, sf::Vector2f pos);

	void update(float deltaTime);

	inline ParticleEmitter& setTexture(sf::Texture texture) { m_texture = texture; return *this; }
	inline ParticleEmitter& velocityFunction(Vec2Function func) { m_velocityFunction = func; return *this; }
	ParticleEmitter& maxParticles(int maxParticles);
	inline ParticleEmitter& maxLifetime(float maxLifetime) { m_maxLifetime = maxLifetime; return *this; }
	ParticleEmitter& spawnTime(float spawnTime);
	inline ParticleEmitter& gravityModifier(float gravityModifier) { m_gravityModifier = gravityModifier; return *this; }
	inline ParticleEmitter& speedModifier(float speedModifier) { m_speedModifier = speedModifier; return *this; }
	inline ParticleEmitter& angularVelocityFunction(const MathFunction& angularVelocityFunction) { m_angularVelocityFunction = angularVelocityFunction; return *this; }
	inline ParticleEmitter& sizeFunction(const Vec2Function& sizeFunction) { m_sizeFunction = sizeFunction; return *this; }
	inline ParticleEmitter& colorFunction(const ColorFunction& colorFunction) { m_colorFunction = colorFunction; return *this; }
protected:
	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

private:
	void spawnParticle();
	void updateQuad(int vertexStart, Particle& p);

private:
	sf::Vector2f m_pos;

	int m_maxParticles;
	int m_numActive;
	std::vector<Particle> m_particles;
	std::vector<sf::Vertex> m_vertices;
	sf::Texture m_texture;

	float m_spawnTime; // in seconds: A new particle will spawn after that time if there is a free slot for it
	float m_spawnTimeRemaining;

	sf::Vector2f m_startSize;

	float m_maxLifetime;

	float m_gravityModifier;
	float m_speedModifier;

	MathFunction m_angularVelocityFunction;
	Vec2Function m_velocityFunction; 
	Vec2Function m_sizeFunction;
	ColorFunction m_colorFunction;
};