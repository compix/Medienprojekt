#include "MenuPageCreateServer.h"
#include "../../Events/CreateServerEvent.h"

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
	tgui::Button::Ptr button = createButton(x2, y, width2, height2, "Create");

	button->bindCallback(&MenuPageCreateServer::onSubmit, this, tgui::Button::LeftMouseClicked);

	m_onShowFocus = m_name.get();
}

void MenuPageCreateServer::onSubmit()
{
	std::string name = m_name->getText();
	std::string host = m_host->getText();
	std::string port = m_port->getText();
	// fixme: validate values

	int portValue = atoi(port.c_str());
	int maxPlayers = 4; // fixme: should the user be able to change this ?

	m_events.emit<CreateServerEvent>(host, portValue, maxPlayers, name);
	//fixme: push lobby page
}