#pragma once

#include <TGUI/TGUI.hpp>

enum MenuCallbacks {
	NONE,

	LOGIN_CLICKED,
};
class Menu
{
public:
	Menu(sf::RenderWindow &window);
	~Menu();

	void handleEvent(sf::Event evt);
	void draw();

private:
	tgui::Gui gui;
	bool visible = true;

	void loadWidgets();
	void loginClicked();
};

