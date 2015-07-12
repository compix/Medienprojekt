#pragma once

#include <stack>
#include <TGUI/TGUI.hpp>
#include <entityx/entityx.h>
#include "Pages/MenuPageRoot.h"
#include "Pages/MenuPageChat.h"
#include "Pages/MenuPageLoading.h"

struct PreloadEvent;
using entityx::Receiver;

class Gui : public tgui::Gui
{
public:
	Gui(sf::RenderWindow &window) : tgui::Gui(window)
	{
		setGlobalFont("Assets/fonts/DejaVuSans.ttf");
	}
};

class Menu : public Receiver<Menu>
{
public:
	Menu();

	void draw();

	void receive(const sf::Event &evt);
	void receive(const PreloadEvent &evt);
	void pushPage(MenuPage *page);
	void popPage();
	void popAllPages();

protected:
	friend class MenuPage;
	Gui m_gui;
	std::stack<MenuPage *> m_pageStack;
	MenuPageLoading m_loadingPage;
	MenuPageRoot m_rootPage;
	MenuPageChat m_chatPage;
};
