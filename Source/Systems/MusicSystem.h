#pragma once
#include <ecstasy/core/EntitySystem.h>

#include <SFML/Audio/Music.hpp>




using namespace ECS;
using sf::Music;

class MusicSystem : public EntitySystem<MusicSystem>
{
public:
	void update(float dt) override;

	void addedToEngine(Engine *engine) override;
	
private:
	void onStartGame();
	void onMusic(const string &name, bool loop);

private:
	Music* m_music;
};

