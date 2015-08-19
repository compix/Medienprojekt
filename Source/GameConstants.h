#pragma once

namespace GameConstants
{
	//Relating to Map/Level
	const float S_SCALE = 32.f;
	const int CELL_WIDTH = 50;
	const int CELL_HEIGHT = 50;
	const int FLOOR_LAYER = -1;
	const int MAIN_LAYER = 0;

	//Relating to Players
	const int MAX_PLAYERS = 4;
	const float PLAYER_SPEED = 4.f;			//Normale Geschwindigkeit des Players
	const float INIT_SPEED_MULTI = 1.f;		//SPEED Multiplikator zu Beginn, !!sollte nicht geringer als 1 sein!!
	const float SPEED_MULTI_CAP = 3.f;		//Grenze des SPEED Multiplikators
	const float SPEED_MULTI_INC = 0.25f;	//Um wie viel die Geschwindigkeit des Players bei jedem Aufsammeln erhoeht wird
	const bool INIT_PLAYERS_CAN_KICK = false;//Ob zu beginn Spieler bereits Bomben kicken können 

	//Relating to Bombs
	const int INIT_BOMB_COUNT = 8;			//Bombenanzahl zu Beginn, !!sollte nicht geringer als 1 sein!!
	const int BOMB_CAP = 8;					//Grenze der Bombenanzahl
	const int INIT_BOMB_RANGE = 1;			//Explosionsreichweite einer Bombe zu Beginn, !!sollte nicht geringer als 1 sein!!
	const int BOMB_RANGE_CAP = 7;			//Explosionsreichweite einer Bombe
	const float KICK_FORCE = 10.f;			//Kraft mit der die Bombe gekickt wird
	const float EXPLOSION_SPREAD_TIME = 0.06f;

	//Relating to Portals
	const float PORTAL_TIMER_NOT_LINKED = 30.f;		//Wenn ein Portal noch nicht verbunden ist, bis es verbunden wird.
	const float PORTAL_TIMER_LINKED = 10.f;			//Wenn zwei Portale stehen

	//Relating to Sounds
	const bool SOUND = true;
	const bool MUSIC = true;

	//Relating to AI
	const float AI_UPDATE_TIME = 1.f / 30.f;

	//Relating to timing Events
	const float LOBBY_COUNTDOWN = 120; // Seconds to count down before forcing all players ready
	const float LOBBY_READY_COUNTDOWN = 3; // Seconds to count down before game begins
	const float RESET_GAME_COUNTDOWN = 2; // Seconds to wait after game over to reset the game

	
}