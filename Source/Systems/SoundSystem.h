#pragma once
#include <ecstasy/core/EntitySystem.h>

#include <SFML/Audio.hpp>


using namespace ECS;

using sf::Sound;
using std::unordered_map;
using std::vector;

class SoundSystem : public EntitySystem<SoundSystem>
{
private:
	typedef std::unordered_map<string, vector<Sound>> SoundMap;
	typedef std::unordered_map<string, vector<bool>> BoolMap;
	SoundMap m_soundsPool;
	BoolMap  m_soundsPlaying;
	unsigned int oldCount = 0;
public:
	void addedToEngine(Engine *engine) override;
	void update(float dt) override;
	
private:
	void onSound(const string &name);
};

