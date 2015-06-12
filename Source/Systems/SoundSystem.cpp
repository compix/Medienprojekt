#include "SoundSystem.h"
#include "../Components/SoundComponent.h"

using namespace entityx;

SoundSystem::SoundSystem()
{
}

void SoundSystem::update(EntityManager& entityManager, EventManager& eventManager, TimeDelta dt)
{
	
	ComponentHandle<SoundComponent> sound;

	for (Entity entity : entityManager.entities_with_components(sound))
	{
		if (sound->destroyEntityAtEnd)
		{
			if (sound->sound.getStatus() == sf::Sound::Playing && !sound->wasPlayedOnce)
			{
				sound->wasPlayedOnce = true;
			}

			if (sound->sound.getStatus() != sf::Sound::Playing && sound->wasPlayedOnce)
			{
				entity.destroy();
			}
		}
	}
}

SoundSystem::~SoundSystem()
{
}
