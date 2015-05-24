#pragma once
#include <entityx/System.h>
#include <Graphics/ParticleManager.h>
#include <Utils/TextureLoader.h>

struct DeathEvent;

class ParticleSystem : public entityx::System<ParticleSystem>, public entityx::Receiver<ParticleSystem>
{
public:
	ParticleSystem(TextureLoader* textureLoader, sf::RenderTarget* renderTarget);
	void update(entityx::EntityManager &entityManager, entityx::EventManager &eventManager, entityx::TimeDelta dt) override;

	ParticleManager* getManager(const std::string& textureName);

	void configure(entityx::EventManager& events) override;

	void receive(const entityx::EntityDestroyedEvent& e);

private:
	void createManager(const std::string& textureName);

private:
	TextureLoader* m_textureLoader;
	sf::RenderTarget* m_renderTarget;

	std::unordered_map<std::string, ParticleManager> m_particleManagers;
};

