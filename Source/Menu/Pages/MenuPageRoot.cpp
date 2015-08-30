#include "MenuPageRoot.h"
#include "../../Events/ExitEvent.h"
#include "../../Events/ForceDisconnectEvent.h"
#include "../Menu.h"
#include "../../GameGlobals.h"
#include "../../Events/CreateGameEvent.h"

MenuPageRoot::MenuPageRoot(Menu &menu)
	:MenuPage(menu), m_createGamePage(menu), m_joinGamePage(menu), m_helpPage(menu), m_settingsPage(menu), m_creditsPage(menu)
{
	createPicture(800, 600, "Assets/ui/background.png");
	auto label = createLabel(470, 20, GameConstants::WINDOW_TITLE);
	label->getLabel()->setTextColor(sf::Color(81, 125, 25));

	auto x = 300.0f;
	auto y = 150.0f;
	auto stepY = 60.0f;
	auto width = 200.0f;
	auto height = 40.0f;

	m_createButton = createButton(x, y, width, height, "Create Game");
	m_createButton->bindCallback(&MenuPageRoot::onCreateGame, this, tgui::Button::LeftMouseClicked);

	y += stepY;
	m_joinButton = createButton(x, y, width, height, "Join Game");
	m_joinButton->bindCallback(&MenuPageRoot::onJoinGame, this, tgui::Button::LeftMouseClicked);

	m_disconnectButton = createButton(x, y, width, height, "Exit to Mainmenu");
	m_disconnectButton->bindCallback(&MenuPageRoot::onDisconnect, this, tgui::Button::LeftMouseClicked);
	m_disconnectButton->hide();

	y += stepY;
	tgui::Button::Ptr button = createButton(x, y, width, height, "Help");
	button->bindCallback(&MenuPageRoot::onHelp, this, tgui::Button::LeftMouseClicked);

	y += stepY;
	button = createButton(x, y, width, height, "Input Helper");
	button->bindCallback(&MenuPageRoot::onSettings, this, tgui::Button::LeftMouseClicked);

	y += stepY;
	button = createButton(x, y, width, height, "Credits");
	button->bindCallback(&MenuPageRoot::onCredits, this, tgui::Button::LeftMouseClicked);

	y += stepY;
	button = createButton(x, y, width, height, "Exit");
	button->bindCallback(&MenuPageRoot::onExit, this, tgui::Button::LeftMouseClicked);

	GameGlobals::events->subscribe<ForceDisconnectEvent>(*this);
	GameGlobals::events->subscribe<StartGameEvent>(*this);
}

void MenuPageRoot::receive(const ForceDisconnectEvent& evt)
{
	m_createButton->show();
	m_joinButton->show();
	m_disconnectButton->hide();
}

void MenuPageRoot::receive(const StartGameEvent& evt)
{
	m_createButton->hide();
	m_joinButton->hide();
	m_disconnectButton->show();
}

void MenuPageRoot::onCreateGame()
{
	m_menu.pushPage(&m_createGamePage);
}

void MenuPageRoot::onJoinGame()
{
	m_menu.pushPage(&m_joinGamePage);
}

void MenuPageRoot::onDisconnect()
{
	GameGlobals::events->emit<ForceDisconnectEvent>();
}

void MenuPageRoot::onHelp()
{
	m_menu.pushPage(&m_helpPage);
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
