#include "MenuPageCreateServer.h"
#include "../../Events/CreateServerEvent.h"
#include "../../GameGlobals.h"
#include "../Menu.h"

MenuPageCreateServer::MenuPageCreateServer(Menu &menu)
	:MenuPage(menu)
{
	createPicture(800, 600, "Assets/ui/xubuntu_bg_aluminium.jpg");
	createLabel(50, 50, "Create Server");

	auto width2 = 300;
	auto height2 = 40;
	auto x1 = 50;
	auto x2 = 250;
	auto y = 200;
	auto stepY = 50;
	auto labelYOffset = 6;

	createLabel(x1, y + labelYOffset, "Name: ");
	m_name = createEditBox(x2, y, width2, height2);
	m_name->setText("Server");

	y += stepY;
	createLabel(x1, y + labelYOffset, "Host/IP: ");
	m_host = createEditBox(x2, y, width2, height2);

	y += stepY;
	createLabel(x1, y + labelYOffset, "Port: ");
	m_port = createEditBox(x2, y, width2, height2);
	m_port->setText("1234");

	y += stepY;
	createLabel(x1, y + labelYOffset, "Width: ");
	m_width = createEditBox(x2, y, width2, height2);
	m_width->setText("21");

	y += stepY;
	createLabel(x1, y + labelYOffset, "Height: ");
	m_height = createEditBox(x2, y, width2, height2);
	m_height->setText("21");

	y += stepY;
	tgui::Button::Ptr button = createButton(x2, y, width2, height2, "Create");

	button->bindCallback(&MenuPageCreateServer::onSubmit, this, tgui::Button::LeftMouseClicked);

	m_onShowFocus = m_name.get();
}

void MenuPageCreateServer::onSubmit()
{
	std::string name = m_name->getText();
	std::string host = m_host->getText();
	std::string port = m_port->getText();
	std::string width = m_width->getText();
	std::string height = m_height->getText();
	// fixme: validate values

	int portValue = atoi(port.c_str());
	int maxPlayers = 4; // fixme: should the user be able to change this ?
	int widthValue = atoi(width.c_str());
	int heightValue = atoi(height.c_str());

	GameGlobals::events->emit<CreateServerEvent>(widthValue, heightValue, host, portValue, maxPlayers, name);
	//fixme: push lobby page
	m_menu.popPage();
	m_menu.popPage();
}