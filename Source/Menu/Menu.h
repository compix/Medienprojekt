#pragma once

#include <stack>
#include <TGUI/TGUI.hpp>
#include "Pages/MenuPageRoot.h"
#include "Pages/MenuPageChat.h"
#include "Pages/MenuPageLoading.h"
#include <signal11/Signal.h>
using Signal11::ScopedConnectionRef;


class Gui : public tgui::Gui
{
public:
	Gui(sf::RenderWindow &window) : tgui::Gui(window)
	{
		setGlobalFont("Assets/fonts/DejaVuSans.ttf");
	}
};

class Menu 
{
public:
	Menu();

	void draw();

	void showLobby();
	void pushPage(MenuPage *page);
	void popPage();
	void popAllPages();

private:
	void onSfml(const sf::Event &evt);
	void onPreload(int progress, int total, string nextSection, const string &nextFilename);
	void onForceDisconnect(const string &message);
	
protected:
	ScopedConnectionRef m_preloadConnection;
	friend class MenuPage;
	Gui m_gui;
	std::stack<MenuPage *> m_pageStack;
	MenuPageLoading m_loadingPage;
	MenuPageRoot m_rootPage;
	MenuPageChat m_chatPage;
	MenuPageLobby m_lobbyPage;
};
