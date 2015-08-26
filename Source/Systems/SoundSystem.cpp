#include "SoundSystem.h"
#include "../GameGlobals.h"
#include "../Utils/AssetManagement/AssetManager.h"

using namespace entityx;
using sf::Sound;
void SoundSystem::update(entityx::EntityManager& entities, entityx::EventManager& events, entityx::TimeDelta dt)
{
}

void SoundSystem::configure(entityx::EventManager& event_manager)
{
	event_manager.subscribe<SoundEvent>(*this);
}

void SoundSystem::receive(const SoundEvent& event)
{
	auto sound = GameGlobals::assetManager->getSound(event.name);
	sound->setVolume(GameConstants::SOUND_VOLUME);
	sound->play();
}