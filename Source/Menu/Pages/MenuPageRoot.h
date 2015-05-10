#pragma once
#include "../MenuPage.h"

class MenuPageRoot : public MenuPage
{
public:
	MenuPageRoot(Menu &menu);

protected:
	void onSingleplayer();
	void onCreateServer();
	void onJoinServer();
	void onSettings();
	void onCredits();
	void onExit();
};
