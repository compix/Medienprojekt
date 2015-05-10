#include "MenuPageRoot.h"
#include "../../Events/ExitEvent.h"

MenuPageRoot::MenuPageRoot(Menu &menu)
	:MenuPage(menu)
{
	createPicture(800, 600, "Assets/ui/xubuntu_bg_aluminium.jpg");

	auto x = 300;
	int y = 120;
	auto stepY = 60;
	auto width = 200;
	auto height = 40;

	tgui::Button::Ptr button = createButton(x, y, width, height, "Single Player");
	button->bindCallback(&MenuPageRoot::onSingleplayer, this, tgui::Button::LeftMouseClicked);

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

void MenuPageRoot::onSingleplayer()
{
	close();
}

void MenuPageRoot::onCreateServer()
{
	close();
}

void MenuPageRoot::onJoinServer()
{
	close();
}

void MenuPageRoot::onSettings()
{
	close();
}

void MenuPageRoot::onCredits()
{
	close();
}

void MenuPageRoot::onExit()
{
	m_events.emit<ExitEvent>();
}
