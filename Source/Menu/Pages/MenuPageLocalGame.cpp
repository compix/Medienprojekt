#include "MenuPageLocalGame.h"
#include "../../Events/CreateLocalGameEvent.h"
#include <format.h>
#include "../../GameGlobals.h"
#include "../Menu.h"

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
	createLabel(x1, y + labelYOffset, "Width: ");
	m_width = createEditBox(x2, y, width2, height2);
	m_width->setText("21");

	y += stepY;
	createLabel(x1, y + labelYOffset, "Height: ");
	m_height = createEditBox(x2, y, width2, height2);
	m_height->setText("21");

	y += stepY;
	tgui::Button::Ptr button = createButton(x2, y, width2, height2, "Create");

	button->bindCallback(&MenuPageLocalGame::onSubmit, this, tgui::Button::LeftMouseClicked);

	m_onShowFocus = m_name[0].get();
}

void MenuPageLocalGame::onSubmit()
{
	std::vector<std::string> names;
	for (int i = 0; i < 4; i++)
	{
		std::string name = m_name[i]->getText();
		if (!name.empty())
			names.push_back(name);
	}
	std::string width = m_width->getText();
	std::string height = m_height->getText();
//	// fixme: validate values

	int widthValue = atoi(width.c_str());
	int heightValue = atoi(height.c_str());
//
	GameGlobals::events->emit<CreateLocalGameEvent>(widthValue, heightValue, names);
	//fixme: push lobby page
	m_menu.popPage();
	m_menu.popPage();
}