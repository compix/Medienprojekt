#pragma once
#include <ecstasy/core/Engine.h>
#include <ecstasy/core/EntitySystem.h>
#include <SFML/Audio/Music.hpp>
#include <signal11/Signal.h>

using sf::Music;
using std::string;

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
	ConnectionScope m_connections;
};

