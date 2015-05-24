#include "MenuPageLocalGame.h"
#include "../../Events/CreateServerEvent.h"
#include <format.h>

MenuPageLocalGame::MenuPageLocalGame(Menu &menu)
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

	for (int i = 0; i < 4; i++)
	{
		createLabel(x1, y + labelYOffset, fmt::format("Player {}:", i+1));
		m_name[i] = createEditBox(x2, y, width2, height2);
		m_name[i]->setText(fmt::format("Player {}", i + 1));
		y += stepY;
	}
	tgui::Button::Ptr button = createButton(x2, y, width2, height2, "Create");

	button->bindCallback(&MenuPageLocalGame::onSubmit, this, tgui::Button::LeftMouseClicked);

	m_onShowFocus = m_name[0].get();
}

void MenuPageLocalGame::onSubmit()
{
//	std::string name = m_name->getText();
//	std::string host = m_host->getText();
//	std::string port = m_port->getText();
//	// fixme: validate values
//
//	int portValue = atoi(port.c_str());
//	int maxPlayers = 4; // fixme: should the user be able to change this ?
//
//	m_events.emit<CreateServerEvent>(host, portValue, maxPlayers, name);
	//fixme: push lobby page
}