#include "MusicSystem.h"
#include "../GameGlobals.h"
#include "../Utils/AssetManagement/AssetManager.h"

void MusicSystem::update(entityx::EntityManager& entities, entityx::EventManager& events, entityx::TimeDelta dt)
{
}

void MusicSystem::configure(entityx::EventManager& event_manager)
{
	event_manager.subscribe<StartGameEvent>(*this);
	event_manager.subscribe<MusicEvent>(*this);
}

void MusicSystem::receive(const StartGameEvent& event)
{
	m_music = GameGlobals::assetManager->getMusic("Title_Theme");
	m_music->setLoop(true);
	m_music->setVolume(50);
	m_music->play();
}

void MusicSystem::receive(const MusicEvent& event)
{
	m_music = GameGlobals::assetManager->getMusic(event.name);
	m_music->setLoop(event.loop);
	m_music->setVolume(50);
	m_music->play();
}

MusicSystem::MusicSystem()
{
}


MusicSystem::~MusicSystem()
{
}
