#pragma once
#include "../MenuPage.h"
#include "MenuPageCreateGame.h"
#include "MenuPageJoinGame.h"
#include "MenuPageSettings.h"
#include "MenuPageCredits.h"

class MenuPageRoot : public MenuPage
{
public:
	MenuPageRoot(Menu &menu);

protected:
	void onCreateGame();
	void onJoinGame();
	void onSettings();
	void onCredits();
	void onExit();

protected:
	MenuPageCreateGame m_createGamePage;
	MenuPageJoinGame m_joinGamePage;
	MenuPageSettings m_settingsPage;
	MenuPageCredits m_creditsPage;
};
