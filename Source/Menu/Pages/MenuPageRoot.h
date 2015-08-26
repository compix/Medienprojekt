#pragma once
#include "../../GameGlobals.h"
#include "../MenuPage.h"
#include "MenuPageCreateGame.h"
#include "MenuPageJoinGame.h"
#include "MenuPageSettings.h"
#include "MenuPageCredits.h"

struct ForceDisconnectEvent;

class MenuPageRoot : public MenuPage, public Receiver<Menu>
{
public:
	MenuPageRoot(Menu &menu);
	void receive(const ForceDisconnectEvent& evt);
	void receive(const StartGameEvent& evt);

	void onEscape() override { if (GameGlobals::game.get()) close(); }

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
	tgui::Button::Ptr m_exitButton;
};
