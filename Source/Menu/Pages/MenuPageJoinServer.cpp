#include "MenuPageJoinServer.h"
#include "../../Events/JoinServerEvent.h"

MenuPageJoinServer::MenuPageJoinServer(Menu &menu)
	:MenuPage(menu)
{
	createPicture(800, 600, "Assets/ui/xubuntu_bg_aluminium.jpg");
	createLabel(50, 50, "Join Server");

	auto width2 = 300;
	auto height2 = 40;
	auto x1 = 50;
	auto x2 = 250;
	auto y = 200;
	auto stepY = 50;
	auto labelYOffset = 6;

	createLabel(x1, y + labelYOffset, "Name: ");
	m_name = createEditBox(x2, y, width2, height2);
	m_name->setText("Client");

	y += stepY;
	createLabel(x1, y + labelYOffset, "Host/IP: ");
	m_host = createEditBox(x2, y, width2, height2);
	m_host->setText("localhost");

	y += stepY;
	createLabel(x1, y + labelYOffset, "Port: ");
	m_port = createEditBox(x2, y, width2, height2);
	m_port->setText("1234");

	y += stepY;
	tgui::Button::Ptr button = createButton(x2, y, width2, height2, "Join");

	button->bindCallback(&MenuPageJoinServer::onSubmit, this, tgui::Button::LeftMouseClicked);

	m_onShowFocus = m_name.get();
}

void MenuPageJoinServer::onSubmit()
{
	std::string name = m_name->getText();
	std::string host = m_host->getText();
	std::string port = m_port->getText();
	// fixme: validate values

	int portValue = atoi(port.c_str());

	m_events.emit<JoinServerEvent>(host, portValue, name);
	//fixme: push lobby page
}