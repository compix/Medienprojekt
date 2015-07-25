#pragma once
#include "Utils/make_unique.h"
#include <SFML/Audio/Music.hpp>

namespace entityx
{
	class EventManager;
	class EntityManager;
}

namespace sf
{
	class RenderWindow;
	class View;
}

class InputManager;
class TextureLoader;
class EntityFactory;
class Game;
class Menu;
class AssetManager;

using std::unique_ptr;

namespace GameGlobals
{
	extern sf::RenderWindow *window;
	extern InputManager *input;
	extern entityx::EventManager *events;
	extern entityx::EntityManager *entities;
	extern EntityFactory *entityFactory;
	extern AssetManager *assetManager;
	extern unique_ptr<Game> game;
	extern sf::View *menuView;
	extern sf::View *screenView;
	extern sf::View *gameView;
};

