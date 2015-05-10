#pragma once
#include "../MenuPage.h"
#include "MenuPageLocalGame.h"
#include "MenuPageCreateServer.h"
#include "MenuPageJoinServer.h"
#include "MenuPageSettings.h"
#include "MenuPageCredits.h"

class MenuPageRoot : public MenuPage
{
public:
	MenuPageRoot(Menu &menu);

protected:
	void onLocalGame();
	void onCreateServer();
	void onJoinServer();
	void onSettings();
	void onCredits();
	void onExit();

protected:
	MenuPageLocalGame m_localGamePage;
	MenuPageCreateServer m_createServerPage;
	MenuPageJoinServer m_joinServerPage;
	MenuPageSettings m_settingsPage;
	MenuPageCredits m_creditsPage;
};
