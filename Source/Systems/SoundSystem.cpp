#include "SoundSystem.h"
#include "../GameGlobals.h"
#include "../Utils/AssetManagement/AssetManager.h"
#include "../Events/PortalCreatedEvent.h"
#include "../Events/ItemPickedUpEvent.h"
#include "../Events/BombExplodedEvent.h"
#include "../Utils/Math.h"
#include "../Components/SoundComponent.h"
#include "../Components/PortalComponent.h"
#include "../Components/BodyComponent.h"
#include "../Components/InputComponent.h"
#include "../Components/BombComponent.h"

using namespace entityx;
using sf::Sound;

void SoundSystem::update(entityx::EntityManager& entities, entityx::EventManager& events, entityx::TimeDelta dt)
{
	// Can be used for step sounds for example. Currently not used because it was too annoying...
	for (auto entity : entities.entities_with_components<SoundComponent, InputComponent>())
	{
		auto soundComponent = entity.component<SoundComponent>();
		auto input = entity.component<InputComponent>();

		bool moving = abs(input->moveX) + abs(input->moveY) > 0; 
		if (!moving)
		{
			soundComponent->remaining *= 0.5f;
			continue;
		}		

		soundComponent->remaining -= float(dt);

		if (soundComponent->remaining > 0.f)
			continue;

		soundComponent->remaining = soundComponent->playFrequency;

		auto sound = GameGlobals::assetManager->getSound(soundComponent->soundName);
		sound->setVolume(soundComponent->volume);
		sound->play();
	}
}

void SoundSystem::configure(entityx::EventManager& event_manager)
{
	event_manager.subscribe<SoundEvent>(*this);
	event_manager.subscribe<entityx::EntityDestroyedEvent>(*this);
	event_manager.subscribe<PortalCreatedEvent>(*this);
	event_manager.subscribe<ItemPickedUpEvent>(*this);
	event_manager.subscribe<BombExplodedEvent>(*this);
}

void SoundSystem::receive(const SoundEvent& soundEvent)
{
	auto sound = GameGlobals::assetManager->getSound(soundEvent.name);
	if (!sound)
		return;
	sound->setVolume(soundEvent.volume);
	sound->play();
}

void SoundSystem::receive(const entityx::EntityDestroyedEvent& entityDestroyedEvent)
{
	auto entity = entityDestroyedEvent.entity;

	if (entity.has_component<PortalComponent>())
		GameGlobals::events->emit<SoundEvent>("portal_close");
}

void SoundSystem::receive(const PortalCreatedEvent& portalCreatedEvent)
{
	GameGlobals::events->emit<SoundEvent>("portal_open");
}

void SoundSystem::receive(const ItemPickedUpEvent& itemPickedUpEvent)
{
	// Not used because too annoying
	//GameGlobals::events->emit<SoundEvent>("item_pickup", GameConstants::SOUND_VOLUME * 0.5f);
}

void SoundSystem::receive(const BombExplodedEvent& bombExplodedEvent)
{
	auto bomb = bombExplodedEvent.bomb;
	
	if (bomb && bomb.has_component<BombComponent>())
	{
		auto bombComponent = bomb.component<BombComponent>();
		switch(bombComponent->type)
		{
		case BombType::NORMAL: 
			GameGlobals::events->emit<SoundEvent>("explosion");
			break;
		case BombType::GHOST:
			GameGlobals::events->emit<SoundEvent>("explosion2");
			break;
		case BombType::LIGHTNING: 
			GameGlobals::events->emit<SoundEvent>("explosion3", GameConstants::SOUND_VOLUME * 0.5f);
			break;
		case BombType::LIGHTNING_PEAK: break;
		default: 
			break;
		}
	}
	
}