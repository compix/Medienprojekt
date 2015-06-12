#include "SoundSystem.h"
#include "../GameGlobals.h"
#include "../Utils/AssetManagement/AssetManager.h"

using namespace entityx;
using sf::Sound;
void SoundSystem::update(entityx::EntityManager& entities, entityx::EventManager& events, entityx::TimeDelta dt)
{
	for (auto it = m_soundsPool.begin(); it != m_soundsPool.end(); ++it)
		for (unsigned i = 0; i < it->second.size(); i++) {
		if (it->second.at(i).getStatus() == sf::SoundSource::Stopped)
			{
				m_soundsPlaying[it->first].at(i) = false;
			}
		#ifdef _DEBUG
			unsigned int count = 0;
			for (bool b : m_soundsPlaying[it->first])
			{
				if (b)
				{
					count++;
				}
			}
			if (count != oldCount){
				oldCount = count;
				cout << "SOUNDS_PLAYING: " << count << endl;
			}
		#endif
		}
}

void SoundSystem::configure(entityx::EventManager& event_manager)
{
	event_manager.subscribe<SoundEvent>(*this);
}

void SoundSystem::receive(const SoundEvent& event)
{
	if (m_soundsPool.find(event.name) == m_soundsPool.end())
	{
		newSound(event.name);
	} else
	{
		bool needNewSound = false;
		for (int i = 0; i < m_soundsPool[event.name].size(); i++)
		{
			if (m_soundsPlaying[event.name].at(i))
			{
				needNewSound = true;
			} else
			{
				m_soundsPool[event.name].at(i).play();
				m_soundsPlaying[event.name].at(i) = true;
				needNewSound = false;
				break;
			}
		}

		if (needNewSound)
		{
			newSound(event.name);
		}
	}
}

void SoundSystem::newSound(string name)
{
	#ifdef _DEBUG
		cout << "NEW SOUND"<< endl;
	#endif
	m_soundsPool[name].insert(m_soundsPool[name].begin(), Sound(*GameGlobals::assetManager->getSound(name)));
	m_soundsPlaying[name].insert(m_soundsPlaying[name].begin(), false);

	m_soundsPool[name].at(0).play();
	m_soundsPlaying[name].at(0) = true;
}


SoundSystem::SoundSystem()
{
}


SoundSystem::~SoundSystem()
{
}
