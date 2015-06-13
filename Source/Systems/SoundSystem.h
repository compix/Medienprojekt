#pragma once
#include <entityx/System.h>
#include "../Events/SoundEvent.h"
#include <SFML/Audio.hpp>

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
private:
	typedef std::unordered_map<string, vector<Sound>> SoundMap;
	typedef std::unordered_map<string, vector<bool>> BoolMap;
	SoundMap m_soundsPool;
	BoolMap  m_soundsPlaying;
	unsigned int oldCount = 0;
public:
	void configure(EventManager& events) override;
	void update(EntityManager &entityManager, EventManager &eventManager, TimeDelta dt) override;
	void SoundSystem::receive(const SoundEvent& event);
};

