#include "MusicSystem.h"
#include "../GameGlobals.h"
#include "../Utils/AssetManagement/AssetManager.h"

void MusicSystem::update(float dt)
{
}

void MusicSystem::addedToEngine(Engine *engine)
{
	event_manager.subscribe<StartGameEvent>(*this);
	event_manager.subscribe<MusicEvent>(*this);
}

void MusicSystem::onStartGame()
{
	m_music = GameGlobals::assetManager->getMusic("Title_Theme");
	m_music->setLoop(true);
	m_music->setVolume(50);
	m_music->play();
}

void MusicSystem::onMusic(const string &name, bool loop)
{
	m_music = GameGlobals::assetManager->getMusic(event.name);
	m_music->setLoop(event.loop);
	m_music->setVolume(50);
	m_music->play();
}
