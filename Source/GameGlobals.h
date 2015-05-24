#pragma once
#include "Utils/make_unique.h"

namespace entityx
{
	class EventManager;
	class EntityManager;
}

namespace sf
{
	class RenderWindow;
}

class InputManager;
class TextureLoader;
class EntityFactory;
class Game;
class Menu;

using std::unique_ptr;

namespace GameGlobals
{
	extern sf::RenderWindow *window;
	extern InputManager *input;
	extern entityx::EventManager *events;
	extern entityx::EntityManager *entities;
	extern EntityFactory *entityFactory;
	extern TextureLoader *textures;
	extern unique_ptr<Game> game;
};

