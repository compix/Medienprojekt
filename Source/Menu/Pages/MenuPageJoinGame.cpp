#include "MenuPageJoinGame.h"
#include "../../Events/JoinGameEvent.h"
#include "../../GameGlobals.h"
#include "../Menu.h"

MenuPageJoinGame::MenuPageJoinGame(Menu &menu)
	:MenuPage(menu), m_connectingPage(menu)
{
	createPicture(800, 600, "Assets/ui/xubuntu_bg_aluminium.jpg");
	createLabel(50, 50, "Join Server");

	auto width2 = 300.0f;
	auto height2 = 40.0f;
	auto x1 = 50.0f;
	auto x2 = 250.0f;
	auto y = 200.0f;
	auto stepY = 50.0f;
	auto labelYOffset = 6.0f;

	createLabel(x1, y + labelYOffset, "Name:");
	m_name = createEditBox(x2, y, width2, height2);
	m_name->setText("Client");

	y += stepY;
	createLabel(x1, y + labelYOffset, "Host/IP:");
	m_host = createEditBox(x2, y, width2, height2);
	m_host->setText("localhost");

	y += stepY;
	createLabel(x1, y + labelYOffset, "UDP Port:");
	m_port = createEditBox(x2, y, width2, height2);
	m_port->setText("1234");

	y += stepY;
	tgui::Button::Ptr button = createButton(x2, y, width2, height2, "Join");

	button->bindCallback(&MenuPageJoinGame::onSubmit, this, tgui::Button::LeftMouseClicked);

	m_onShowFocus = m_name.get();
}

void MenuPageJoinGame::onSubmit()
{
	std::string name = m_name->getText();
	std::string host = m_host->getText();
	std::string port = m_port->getText();
	// fixme: validate values

	int portValue = atoi(port.c_str());

	m_menu.pushPage(&m_connectingPage);
	GameGlobals::events->emit<JoinGameEvent>(host, portValue, name);
}