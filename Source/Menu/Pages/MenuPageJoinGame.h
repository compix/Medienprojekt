#pragma once
#include "../MenuPage.h"
#include "MenuPageConnecting.h"
#include "MenuPageLobby.h"

class MenuPageJoinGame : public MenuPage
{
public:
	MenuPageJoinGame(Menu &menu);
	void onSubmit();
private:
	MenuPageConnecting m_connectingPage;
	tgui::EditBox::Ptr m_name;
	tgui::EditBox::Ptr m_host;
	tgui::EditBox::Ptr m_port;
};
