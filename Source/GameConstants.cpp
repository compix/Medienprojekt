#include "GameConstants.h"

namespace GameConstants
{
	// Relating to Map/Level
	extern const float S_SCALE = 32.f;
	extern const int CELL_WIDTH = 50;
	extern const int CELL_HEIGHT = 50;
	extern const int FLOOR_LAYER = -1;
	extern const int MAIN_LAYER = 0;
	extern const int JUMP_LAYER = 1;

	// Relating to Players
	extern const float PLAYER_SPEED = 4.f;
	extern const float INIT_SPEED_MULTI = 1.f;
	extern const float SPEED_MULTI_CAP = 2.5f;
	extern const float SPEED_MULTI_INC = 0.25f;
	extern const bool INIT_PLAYERS_CAN_KICK = false;
	extern const bool INIT_PLAYERS_CAN_HOLD_BOMB = false;
	extern const bool INIT_PORTAL_SKILL = false;
	extern const bool INIT_ANTI_MAGNET = false;
	extern const bool INIT_PUNCH_SKILL = false;
	extern const bool INIT_BLINK_SKILL = true;
	extern const int PUNCH_DISTANCE = 2;
	extern const float BLINK_SPEED = 20.f;

	//Relating to Punch/Jumping
	extern const float EARTH_GRAVITY = 9.81f;
	extern const float PUNCH_JUMPING_SPEED = 8;
	extern const float BLOCKED_CELL_JUMPING_SPEED = 8;
	extern const float PUNCH_JUMPING_HEIGHT = 6;
	extern const float BLOCKED_CELL_JUMPING_HEIGHT = 6;


	// Relating to Bombs
	extern const int INIT_BOMB_COUNT = 1;
	extern const int BOMB_CAP = 8;
	extern const int INIT_BOMB_RANGE = 2;
	extern const int BOMB_RANGE_CAP = 10;
	extern const float KICK_FORCE = 10.f;
	extern const float EXPLOSION_SPREAD_TIME = 0.06f;
	extern const float EXPLOSION_TIME = 2.f;

	// Relating to Portals
	extern const float PORTAL_TIMER_NOT_LINKED = 30.f;
	extern const float PORTAL_TIMER_LINKED = 10.f;

	// Relating to Sounds
	extern const bool SOUND = true;
	extern const bool MUSIC = true;
	extern const float MUSIC_VOLUME = 100.f;
	extern const float SOUND_VOLUME = 50.f;

	// Relating to AI
	extern const float AI_UPDATE_TIME = 1.f / 30.f;

	// Relating to timing Events
	extern const float LOBBY_COUNTDOWN = 120;
	extern const float LOBBY_READY_COUNTDOWN = 3;
	extern const float RESET_GAME_COUNTDOWN = 2;
}

