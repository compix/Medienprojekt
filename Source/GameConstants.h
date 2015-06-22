#pragma once

namespace GameConstants
{
	const int CELL_WIDTH = 50;
	const int CELL_HEIGHT = 50;
	const float S_SCALE = 32.f;
	const float PLAYER_SPEED = 4.f;			//Normale Geschwindigkeit des Players

	const int BOMB_CAP = 8;					//Grenze der Bombenanzahl
	const float SPEED_MULTI_CAP = 3.f;		//Grenze des SPEED Multiplikators
	const float SPEED_MULTI_INC = 0.25f;	//Um wie viel die Geschwindigkeit des Players bei jedem Aufsammeln erhöht wird
	const float KICK_FORCE = 10.f;			//Kraft mit der die Bombe gekickt wird

	const int FLOOR_LAYER = -1;
	const int MAIN_LAYER = 0;

	const bool SOUND = true;
	const bool MUSIC = true;
}