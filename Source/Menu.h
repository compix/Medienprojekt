#pragma once

#include <TGUI/TGUI.hpp>
#include <entityx/entityx.h>
#include "Events/ChatEvent.h"

enum class MenuState : uint8_t
{
	CLOSED,

	OPEN,
	CHAT
};

using entityx::Receiver;
using entityx::EventManager;

class Menu : public Receiver<Menu>
{
public:
	Menu(sf::RenderWindow &window, EventManager &events);

	void draw();

	void receive(const sf::Event &evt);
	void receive(const ChatEvent &evt);

private:
	tgui::Gui m_gui;
	tgui::Gui m_chatGui;
	MenuState m_state = MenuState::CLOSED;
	EventManager &events;

	void loadWidgets();
	void loginClicked();
	void chatSend();
	void setState(MenuState state);
};

