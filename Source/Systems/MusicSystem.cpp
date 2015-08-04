#include "MusicSystem.h"
#include "../GameGlobals.h"
#include <ecstasy/core/Engine.h>
#include "../Utils/AssetManagement/AssetManager.h"

void MusicSystem::update(float dt)
{
}

void MusicSystem::addedToEngine(Engine *engine)
{
	m_connections += GameGlobals::events->startGame.connect(this, MusicSystem::onStartGame);
	m_connections += GameGlobals::events->music.connect(this, MusicSystem::onMusic);
}

void MusicSystem::onStartGame()
{
//	m_music = GameGlobals::assetManager->getMusic("Title_Theme");
//	m_music->setLoop(true);
//	m_music->setVolume(50);
//	m_music->play();
}

void MusicSystem::onMusic(const string &name, bool loop)
{
//	m_music = GameGlobals::assetManager->getMusic(name);
//	m_music->setLoop(loop);
//	m_music->setVolume(50);
//	m_music->play();
}
