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

	bool handleEvent(const sf::Event &evt);
	void draw();

private:
	tgui::Gui m_gui;
	bool m_visible = true;

	void loadWidgets();
	void loginClicked();
};

