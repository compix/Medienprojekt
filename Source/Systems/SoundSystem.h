#pragma once
#include <entityx/System.h>
#include "../Events/SoundEvent.h"
#include <SFML/Audio.hpp>

struct BombExplodedEvent;
struct ItemPickedUpEvent;
struct PortalCreatedEvent;
struct SoundEvent;
using entityx::System;
using entityx::Receiver;
using entityx::EntityManager;
using entityx::EventManager;
using entityx::TimeDelta;
using sf::Sound;
using std::unordered_map;
using std::vector;

class SoundSystem : public System<SoundSystem>, public Receiver<SoundSystem>
{
public:
	void configure(EventManager& events) override;
	void update(EntityManager &entityManager, EventManager &eventManager, TimeDelta dt) override;
	void receive(const SoundEvent& soundEvent);
	void receive(const entityx::EntityDestroyedEvent& entityDestroyedEvent);
	void receive(const PortalCreatedEvent& portalCreatedEvent);
	void receive(const ItemPickedUpEvent& itemPickedUpEvent);
	void receive(const BombExplodedEvent& bombExplodedEvent);

private:
	typedef std::unordered_map<string, vector<Sound>> SoundMap;
	typedef std::unordered_map<string, vector<bool>> BoolMap;
	SoundMap m_soundsPool;
	BoolMap  m_soundsPlaying;
	unsigned int oldCount = 0;
};

