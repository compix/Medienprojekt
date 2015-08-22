#pragma once

namespace GameConstants
{
	// Relating to Map/Level
	extern const float S_SCALE;
	extern const int CELL_WIDTH;
	extern const int CELL_HEIGHT;
	extern const int FLOOR_LAYER;
	extern const int MAIN_LAYER;
	extern const int JUMP_LAYER;

	// Relating to Players
	const int MAX_PLAYERS = 4;
	extern const float PLAYER_SPEED;			// Normale Geschwindigkeit des Players
	extern const float INIT_SPEED_MULTI;		// SPEED Multiplikator zu Beginn, !!sollte nicht geringer als 1 sein!!
	extern const float SPEED_MULTI_CAP;			// Grenze des SPEED Multiplikators
	extern const float SPEED_MULTI_INC;			// Um wie viel die Geschwindigkeit des Players bei jedem Aufsammeln erhoeht wird
	extern const bool INIT_PLAYERS_CAN_KICK;	// Ob zu beginn Spieler bereits Bomben kicken können 
	extern const bool INIT_PLAYERS_CAN_HOLD_BOMB;
	extern const bool INIT_PORTAL_SKILL;		// Ob zu beginn von Spielern Portale gesetzt werden können.
	extern const bool INIT_ANTI_MAGNET;			// Ob gekickte Bomben von Beginn an von Spielern abprallen.
	extern const bool INIT_PUNCH_SKILL;		//Ob Spieler zu beginn Bomben schlagen können.
	extern const int PUNCH_DISTANCE;

	//Relating to Punch/Jumping
	extern const float EARTH_GRAVITY;
	extern const float PUNCH_JUMPING_SPEED;
	extern const float BLOCKED_CELL_JUMPING_SPEED;
	extern const float PUNCH_JUMPING_HEIGHT;
	extern const float BLOCKED_CELL_JUMPING_HEIGHT;

	// Relating to Bombs
	extern const int INIT_BOMB_COUNT;			// Bombenanzahl zu Beginn, !!sollte nicht geringer als 1 sein!!
	extern const int BOMB_CAP;					// Grenze der Bombenanzahl
	extern const int INIT_BOMB_RANGE;			// Explosionsreichweite einer Bombe zu Beginn, !!sollte nicht geringer als 1 sein!!
	extern const int BOMB_RANGE_CAP;			// Explosionsreichweite einer Bombe
	extern const float KICK_FORCE;				// Kraft mit der die Bombe gekickt wird
	extern const float EXPLOSION_SPREAD_TIME;	// Time in seconds the explosion needs to spread to the next cell
	extern const float EXPLOSION_TIME;			// Time in seconds for a bomb to explode

	// Relating to Portals
	extern const float PORTAL_TIMER_NOT_LINKED;	// Wenn ein Portal noch nicht verbunden ist, bis es verbunden wird.
	extern const float PORTAL_TIMER_LINKED;		// Wenn zwei Portale stehen

	// Relating to Sounds
	extern const bool SOUND;
	extern const bool MUSIC;

	// Relating to AI
	extern const float AI_UPDATE_TIME;

	// Relating to timing Events
	extern const float LOBBY_COUNTDOWN;			// Seconds to count down before forcing all players ready
	extern const float LOBBY_READY_COUNTDOWN;	// Seconds to count down before game begins
	extern const float RESET_GAME_COUNTDOWN;	// Seconds to wait after game over to reset the game	
}