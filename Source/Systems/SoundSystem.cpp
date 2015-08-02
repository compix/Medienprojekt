#include "SoundSystem.h"
#include "../GameGlobals.h"
#include "../Utils/AssetManagement/AssetManager.h"

using sf::Sound;
void SoundSystem::update(float dt)
{
}

void SoundSystem::addedToEngine(Engine *engine)
{
	event_manager.subscribe<SoundEvent>(*this);
}

void SoundSystem::onSound(const string &name)
{
	GameGlobals::assetManager->getSound(name)->play();
}