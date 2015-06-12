#pragma once
#include <entityx/Event.h>
#include <entityx/System.h>
#include "../Events/MusicEvent.h"
#include <SFML/Audio/Music.hpp>


struct MusicEvent;
struct StartGameEvent;
using entityx::System;
using entityx::Receiver;
using sf::Music;

class MusicSystem : public System<MusicSystem>, public Receiver<MusicSystem>
{
public:
	void update(entityx::EntityManager& entities, entityx::EventManager& events, entityx::TimeDelta dt) override;

	void configure(entityx::EventManager &event_manager) override;
	void receive(const StartGameEvent& event);
	void receive(const MusicEvent& event);
	MusicSystem();
	~MusicSystem();

private:
	Music* m_music;
};

