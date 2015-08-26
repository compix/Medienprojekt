#include "MenuPageRoot.h"
#include "../../Events/ExitEvent.h"
#include "../Menu.h"
#include "../../GameGlobals.h"

MenuPageRoot::MenuPageRoot(Menu &menu)
	:MenuPage(menu), m_createGamePage(menu), m_joinGamePage(menu), m_settingsPage(menu), m_creditsPage(menu)
{
	createPicture(800, 600, "Assets/ui/xubuntu_bg_aluminium.jpg");

	auto x = 300.0f;
	auto y = 150.0f;
	auto stepY = 60.0f;
	auto width = 200.0f;
	auto height = 40.0f;

	tgui::Button::Ptr button = createButton(x, y, width, height, "Create Game");
	button->bindCallback(&MenuPageRoot::onCreateGame, this, tgui::Button::LeftMouseClicked);

	y += stepY;
	button = createButton(x, y, width, height, "Join Game");
	button->bindCallback(&MenuPageRoot::onJoinGame, this, tgui::Button::LeftMouseClicked);

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

void MenuPageRoot::onCreateGame()
{
	m_menu.pushPage(&m_createGamePage);
}

void MenuPageRoot::onJoinGame()
{
	m_menu.pushPage(&m_joinGamePage);
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
