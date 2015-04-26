#include "Game.h"
#include "AnimationSystem.h"
#include "RenderSystem.h"

Game::Game(sf::RenderWindow* pWindow)
{
	systems.add<AnimationSystem>();
	systems.add<RenderSystem>(pWindow);
	systems.configure();
}


Game::~Game()
{
}

void Game::update(TimeDelta dt)
{
	systems.update<AnimationSystem>(dt);
	systems.update<RenderSystem>(dt);
}
