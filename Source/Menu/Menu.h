#pragma once

#include <stack>
#include <TGUI/TGUI.hpp>
#include <entityx/entityx.h>
#include "Pages/MenuPageRoot.h"
#include "Pages/MenuPageChat.h"

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
	void pushPage(MenuPage *page);
	void popPage();

protected:
	friend class MenuPage;
	Gui m_gui;
	std::stack<MenuPage *> m_pageStack;
	MenuPageRoot m_rootPage;
	MenuPageChat m_chatPage;
};
