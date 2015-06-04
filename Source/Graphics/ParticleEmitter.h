#pragma once
#include "Particle.h"
#include <vector>
#include <SFML/Graphics.hpp>
#include <functional>
#include "../Utils/Math.h"
#include <cinttypes>
#include "../Utils/AssetManagement/TexturePacker.h"

struct RGB;
const float PARTICLE_GRAVITY = 9.81f;

typedef std::function < float(float) > MathFunction; // returns f(time)
typedef std::function < sf::Vector2f(float) > Vec2Function; // returns 2d vector f(time)
typedef std::function < sf::Vector3f(float) > Vec3Function; // returns 3d vector f(time)
typedef std::function < RGB(float) > ColorFunction; // returns RGB f(time)

class ParticleManager;

/**
 * @brief	Functions are used to manipulate particles: Input is the time (lifetime of a particle) in range ~[0, 1]. Output depends on the function.
 * 			If t = 0 then the particle just spawned and if t = 1 then it is dead.
 */
class ParticleEmitter : public sf::Drawable
{
	friend class ParticleManager;
public:
	ParticleEmitter();
	ParticleEmitter(uint32_t maxParticles);

	void update(float deltaTime);
	void update(Particle& p, float deltaTime, sf::Color& colorOut, sf::Vector2f& sizeOut);

	inline ParticleEmitter& velocityFunction(Vec2Function func) { m_velocityFunction = func; return *this; }
	//ParticleEmitter& maxParticles(int maxParticles);
	inline ParticleEmitter& maxLifetime(float maxLifetime) { m_maxLifetime = maxLifetime; return *this; }
	ParticleEmitter& spawnTime(float spawnTime);
	inline ParticleEmitter& gravityModifier(float gravityModifier) { m_gravityModifier = gravityModifier; return *this; }
	inline ParticleEmitter& speedModifier(float speedModifier) { m_speedModifier = speedModifier; return *this; }
	inline ParticleEmitter& angularVelocityFunction(const MathFunction& angularVelocityFunction) { m_angularVelocityFunction = angularVelocityFunction; return *this; }
	inline ParticleEmitter& sizeFunction(const Vec2Function& sizeFunction) { m_sizeFunction = sizeFunction; return *this; }
	inline ParticleEmitter& colorFunction(const ColorFunction& colorFunction) { m_colorFunction = colorFunction; return *this; }
	inline ParticleEmitter& transparencyFunction(const MathFunction& transparencyFunction) { m_transparencyFunction = transparencyFunction; return *this; }
	ParticleEmitter& burstTime(float burstTime);
	ParticleEmitter& burstParticleNumber(int num);
	inline ParticleEmitter& position(sf::Vector2f pos) { m_pos = pos; return *this; }
	inline ParticleEmitter& position(float x, float y) { m_pos.x = x; m_pos.y = y; return *this; }
	inline ParticleEmitter& rotation(float rotation) { m_rotation = Math::toRadians(rotation); return *this; }
	inline ParticleEmitter& spawnWidth(float width) { m_spawnWidth = width; return *this; }
	inline ParticleEmitter& spawnHeight(float height) { m_spawnHeight = height; return *this; }
	// negative = infinity
	inline ParticleEmitter& burstNumber(int burstNumber) { m_burstNumber = burstNumber; return *this; }
	inline ParticleEmitter& blendMode(const sf::BlendMode& blendMode) { m_blendMode = blendMode; return *this; }
	// default: FLT_MAX
	inline ParticleEmitter& spawnDuration(float spawnDuration) { m_spawnDuration = spawnDuration; return *this; }

	inline void remove() { m_scheduledForRemoval = true; }
	inline bool alive() { return !m_scheduledForRemoval; }
	void refresh();

	void setTexture(Assets::Texture* texture);
private:
	void spawnParticle();

	void updateQuad(int vertexStart, Particle& p, const sf::Color& color, const sf::Vector2f& size);
protected:
	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
private:
	Assets::Texture* m_texture;
	uint32_t m_numActive;

	sf::Vector2f m_pos;
	float m_rotation;
	std::vector<Particle> m_particles;
	std::vector<sf::Vertex> m_vertices;

	float m_spawnTime; // in seconds: A new particle will spawn after that time if there is a free slot for it
	float m_spawnTimeRemaining;

	float m_burstTime;
	float m_burstTimeRemaining;
	int m_burstParticleNumber;
	int m_burstNumber;

	sf::Vector2f m_startSize;

	float m_maxLifetime;

	float m_gravityModifier;
	float m_speedModifier;

	float m_spawnWidth;
	float m_spawnHeight;

	float m_spawnDuration;
	sf::BlendMode m_blendMode;

	MathFunction m_angularVelocityFunction;
	Vec2Function m_velocityFunction; 
	Vec2Function m_sizeFunction;
	ColorFunction m_colorFunction;
	MathFunction m_transparencyFunction;

	bool m_scheduledForRemoval;
};