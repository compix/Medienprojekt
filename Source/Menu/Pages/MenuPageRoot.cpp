#include "MenuPageRoot.h"
#include "../../Events/ExitEvent.h"
#include "../../Events/ForceDisconnectEvent.h"
#include "../Menu.h"
#include "../../GameGlobals.h"
#include "../../Events/CreateGameEvent.h"

MenuPageRoot::MenuPageRoot(Menu &menu)
	:MenuPage(menu), m_createGamePage(menu), m_joinGamePage(menu), m_helpPage(menu), m_creditsPage(menu)
{
	createPicture(800, 600, "Assets/ui/background.png");

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
	button = createButton(x, y, width, height, "Help");
	button->bindCallback(&MenuPageRoot::onHelp, this, tgui::Button::LeftMouseClicked);

	y += stepY;
	button = createButton(x, y, width, height, "Credits");
	button->bindCallback(&MenuPageRoot::onCredits, this, tgui::Button::LeftMouseClicked);

	y += stepY;
	m_exitButton = createButton(x, y, width, height, "Exit");
	m_exitButton->bindCallback(&MenuPageRoot::onExit, this, tgui::Button::LeftMouseClicked);

	GameGlobals::events->subscribe<ForceDisconnectEvent>(*this);
	GameGlobals::events->subscribe<StartGameEvent>(*this);
}

void MenuPageRoot::receive(const ForceDisconnectEvent& evt)
{
	m_exitButton->setText("Exit");
}

void MenuPageRoot::receive(const StartGameEvent& evt)
{
	m_exitButton->setText("Exit to Mainmenu");
}

void MenuPageRoot::onCreateGame()
{
	m_menu.pushPage(&m_createGamePage);
}

void MenuPageRoot::onJoinGame()
{
	m_menu.pushPage(&m_joinGamePage);
}

void MenuPageRoot::onHelp()
{
	m_menu.pushPage(&m_helpPage);
}

void MenuPageRoot::onCredits()
{
	m_menu.pushPage(&m_creditsPage);
}

void MenuPageRoot::onExit()
{
	if (GameGlobals::game.get())
		GameGlobals::events->emit<ForceDisconnectEvent>();
	else
		GameGlobals::events->emit<ExitEvent>();
}
