#include "MenuPageRoot.h"
#include "../../Events/ExitEvent.h"
#include "../Menu.h"
#include "../../GameGlobals.h"

MenuPageRoot::MenuPageRoot(Menu &menu)
	:MenuPage(menu), m_localGamePage(menu), m_createServerPage(menu), m_joinServerPage(menu), m_settingsPage(menu), m_creditsPage(menu)
{
	createPicture(800, 600, "Assets/ui/xubuntu_bg_aluminium.jpg");

	auto x = 300;
	int y = 120;
	auto stepY = 60;
	auto width = 200;
	auto height = 40;

	tgui::Button::Ptr button = createButton(x, y, width, height, "Local Game");
	button->bindCallback(&MenuPageRoot::onLocalGame, this, tgui::Button::LeftMouseClicked);

	y += stepY;
	button = createButton(x, y, width, height, "Create Server");
	button->bindCallback(&MenuPageRoot::onCreateServer, this, tgui::Button::LeftMouseClicked);

	y += stepY;
	button = createButton(x, y, width, height, "Join Server");
	button->bindCallback(&MenuPageRoot::onJoinServer, this, tgui::Button::LeftMouseClicked);

	y += stepY;
	button = createButton(x, y, width, height, "Settings");
	button->bindCallback(&MenuPageRoot::onSettings, this, tgui::Button::LeftMouseClicked);

	y += stepY;
	button = createButton(x, y, width, height, "Credits");
	button->bindCallback(&MenuPageRoot::onCredits, this, tgui::Button::LeftMouseClicked);

	y += stepY;
	button = createButton(x, y, width, height, "Exit");
	button->bindCallback(&MenuPageRoot::onExit, this, tgui::Button::LeftMouseClicked);
}

void MenuPageRoot::onLocalGame()
{
	m_menu.pushPage(&m_localGamePage);
}

void MenuPageRoot::onCreateServer()
{
	m_menu.pushPage(&m_createServerPage);
}

void MenuPageRoot::onJoinServer()
{
	m_menu.pushPage(&m_joinServerPage);
}

void MenuPageRoot::onSettings()
{
	m_menu.pushPage(&m_settingsPage);
}

void MenuPageRoot::onCredits()
{
	m_menu.pushPage(&m_creditsPage);
}

void MenuPageRoot::onExit()
{
	GameGlobals::events->emit<ExitEvent>();
}
