#pragma once

namespace GameConstants
{
	const int CELL_WIDTH = 50;
	const int CELL_HEIGHT = 50;
	const float S_SCALE = 32.f;
	const float PLAYER_SPEED = 4.f;			//Normale Geschwindigkeit des Players
	const int MAX_PLAYERS = 4;

	const int BOMB_CAP = 8;					//Grenze der Bombenanzahl
	const float SPEED_MULTI_CAP = 3.f;		//Grenze des SPEED Multiplikators
	const float SPEED_MULTI_INC = 0.25f;	//Um wie viel die Geschwindigkeit des Players bei jedem Aufsammeln erhöht wird
	const float KICK_FORCE = 10.f;			//Kraft mit der die Bombe gekickt wird

	const int FLOOR_LAYER = -1;
	const int MAIN_LAYER = 0;

	const bool SOUND = true;
	const bool MUSIC = true;

	const float PORTAL_TIMER_LINKED = 10.f;			//Wenn zwei Portale stehen
	const float PORTAL_TIMER_NOT_LINKED = 30.f;		//Wenn ein Portal noch nicht verbunden ist, bis es verbunden wird.
}