#pragma once
#include "../../GameGlobals.h"
#include "../MenuPage.h"
#include "MenuPageCreateGame.h"
#include "MenuPageJoinGame.h"
#include "MenuPageHelp.h"
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
	void onDisconnect();
	void onHelp();
	void onCredits();
	void onExit();

protected:
	MenuPageCreateGame m_createGamePage;
	MenuPageJoinGame m_joinGamePage;
	MenuPageHelp m_helpPage;
	MenuPageCredits m_creditsPage;
	tgui::Button::Ptr m_createButton;
	tgui::Button::Ptr m_joinButton;
	tgui::Button::Ptr m_disconnectButton;
};
