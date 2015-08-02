#pragma once
#include "Utils/make_unique.h"
#include "Network/NetPlayerInfo.h"
#include <SFML/Audio/Music.hpp>
#include <signal11/Signal.h>
#include <ecstasy/core/Entity.h>
#include "Utils/Common.h"
#include "Components/ItemComponent.h"
#include "GameConstants.h"

using std::string;
using std::vector;

namespace sf
{
	class RenderWindow;
	class View;
	class Event;
}

class InputManager;
class TextureLoader;
class EntityFactory;
class Game;
class Menu;
class AssetManager;

using std::unique_ptr;
enum class ClientState
{
	CONNECTING,
	CONNECTED,
	LOBBY,
	PREGAME,
	DISCONNECTED
};

struct CreateGamePlayerInfo
{
	explicit CreateGamePlayerInfo(const string &name, CreateGamePlayerType type) : name(name), type(type) {}
	string name;
	CreateGamePlayerType type;
};
struct LobbyEvent
{
	LobbyEvent(int numPlayers)
		: numPlayers(numPlayers)
	{
		for (int i = 0; i < numPlayers; i++)
		{
			enabled[i] = false;
			ready[i] = false;
		}
	}

	int numPlayers;
	std::string name[GameConstants::MAX_PLAYERS];
	bool enabled[GameConstants::MAX_PLAYERS];
	bool ready[GameConstants::MAX_PLAYERS];
};
using Signal11::Signal;
using ECS::Entity;
struct GameEvents
{
	Signal<void (const sf::Event &evt)> sfml;
	Signal<void (Entity *entity, uint8_t x, uint8_t y, Entity *owner)> bombCreated;
	Signal<void (Entity *bomb)> bombExploded;
	Signal<void (Entity *entity, uint8_t x, uint8_t y, Entity *target)> boostEffectCreated;
	Signal<void (const string &message, const string &name)> chat;
	Signal<void (const string &message, ClientState state)> clientState;
	Signal<void (const string &message)> connect;
	Signal<void (const string &name, CreateGamePlayerType type)> countdown;
	Signal<void (uint8_t width, uint8_t height, const vector<CreateGamePlayerInfo> &players, int port, int maxClients)> createGame;
	Signal<void (Entity *triggerEntity)> createPortal;
	Signal<void (Entity *dyingEntity)> death;
	Signal<void (const string &reason, NetPlayerInfo *playerInfo)> disconnect;
	Signal<void (Entity *damageDealer, Entity *damagedEntity, int damage)> entityGotHit;
	Signal<void ()> exit;
	Signal<void (Entity *entity, uint8_t x, uint8_t y, Direction direction, uint8_t range, float spreadTime)> explosionCreated;
	Signal<void (const string &message)> forceDisconnect;
	Signal<void (Entity *entity, uint8_t x, uint8_t y, ItemType type)> itemCreated;
	Signal<void (Entity *item, Entity *itemReceiver)> itemPickedUp;
	Signal<void (const string &host, int port, const string &name)> joinGame;
	Signal<void (LobbyEvent &evt)> lobby;
	Signal<void ()> lobbyDisable;
	Signal<void (bool visible)> menuShow;
	Signal<void (const string &name, bool loop)> music;
	Signal<void (uint8_t playerIndex, const string &name)> playerJoin;
	Signal<void (Entity *entity, uint8_t x, uint8_t y, Entity *owner)> portalCreated;
	Signal<void (int progress, int total, string nextSection, const string &nextFilename)> preload;
	Signal<void (uint8_t playerIndex, bool ready)> ready;
	Signal<void (const string &message)> sendChat;
	Signal<void (int playerIndex, bool ready)> setReady;
	Signal<void (Entity *entity, uint8_t x, uint8_t y)> smokeCreated;
	Signal<void (const string &name)> sound;
	Signal<void ()> startGame;
	Signal<void (Entity *affectedEntity)> timeout;
};

namespace GameGlobals
{
	extern sf::RenderWindow *window;
	extern InputManager *input;
	extern GameEvents *events;
	extern EntityFactory *entityFactory;
	extern AssetManager *assetManager;
	extern unique_ptr<Game> game;
	extern sf::View *menuView;
	extern sf::View *screenView;
	extern sf::View *gameView;
};

