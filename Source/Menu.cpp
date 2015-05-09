#include "Menu.h"
#include "Events/ChatEvent.h"
#include "Events/SendChatEvent.h"
#include "Events/MenuShowEvent.h"

Menu::Menu(sf::RenderWindow &window, EventManager &events) : m_gui(window), m_chatGui(window), events(events)
{
	events.subscribe<sf::Event>(*this);
	events.subscribe<ChatEvent>(*this);

	// Load the font (you should check the return value to make sure that it is loaded)
	m_gui.setGlobalFont("Assets/fonts/DejaVuSans.ttf");
	m_chatGui.setGlobalFont("Assets/fonts/DejaVuSans.ttf");

	// Load the widgets
	loadWidgets();
}

void Menu::loadWidgets()
{
	// Create the background image
	tgui::Picture::Ptr picture(m_gui);
	picture->load("Assets/ui/xubuntu_bg_aluminium.jpg");
	picture->setSize(800, 600);

	// Create the username label
	tgui::Label::Ptr labelUsername(m_gui);
	labelUsername->setText("Username:");
	labelUsername->setPosition(200, 100);

	// Create the password label
	tgui::Label::Ptr labelPassword(m_gui);
	labelPassword->setText("Password:");
	labelPassword->setPosition(200, 250);

	// Create the username edit box
	tgui::EditBox::Ptr editBoxUsername(m_gui, "Username");
	editBoxUsername->load("Assets/ui/widgets/Black.conf");
	editBoxUsername->setSize(400, 40);
	editBoxUsername->setPosition(200, 140);

	// Create the password edit box (we will copy the previously created edit box)
	tgui::EditBox::Ptr editBoxPassword = m_gui.copy(editBoxUsername, "Password");
	editBoxPassword->setPosition(200, 290);
	editBoxPassword->setPasswordCharacter('*');

	// Create the login button
	tgui::Button::Ptr button(m_gui);
	button->load("Assets/ui/widgets/Black.conf");
	button->setSize(260, 60);
	button->setPosition(270, 440);
	button->setText("Login");
	button->bindCallback(&Menu::loginClicked, this, tgui::Button::LeftMouseClicked);

	// Chat
	tgui::ChatBox::Ptr chatBox(m_chatGui, "ChatBox");
	chatBox->load("Assets/ui/widgets/Black.conf");
	chatBox->setSize(760, 500);
	chatBox->setPosition(20, 20);

	tgui::EditBox::Ptr editBoxChat(m_chatGui, "ChatEdit");
	editBoxChat->load("Assets/ui/widgets/Black.conf");
	editBoxChat->setSize(760, 40);
	editBoxChat->setPosition(20, 540);
	editBoxChat->bindCallback(&Menu::chatSend, this, tgui::EditBox::ReturnKeyPressed);
}

void Menu::loginClicked()
{
	// Get the username and password
	tgui::EditBox::Ptr editBoxUsername = m_gui.get("Username");
	tgui::EditBox::Ptr editBoxPassword = m_gui.get("Password");

	sf::String username = editBoxUsername->getText();
	sf::String password = editBoxPassword->getText();
	setState(MenuState::CLOSED);
}

void Menu::chatSend()
{
	tgui::EditBox::Ptr editBoxChat = m_chatGui.get("ChatEdit");

	sf::String message = editBoxChat->getText();
	events.emit<SendChatEvent>(message);
	editBoxChat->setText("");
}

void Menu::setState(MenuState state)
{
	m_state = state;
	events.emit<MenuShowEvent>(m_state != MenuState::CLOSED);
}

void Menu::draw()
{
	if (m_state == MenuState::OPEN)
		m_gui.draw();
	else if (m_state == MenuState::CHAT)
		m_chatGui.draw();
}

void Menu::receive(const sf::Event &evt)
{
	if (m_state == MenuState::OPEN)
	{
		if (evt.type == sf::Event::KeyPressed && evt.key.code == sf::Keyboard::Escape)
			setState(MenuState::CLOSED);
		else
			m_gui.handleEvent(evt);
	}
	else if (m_state == MenuState::CHAT)
	{
		if (evt.type == sf::Event::KeyPressed && evt.key.code == sf::Keyboard::Escape)
			setState(MenuState::CLOSED);
		else
			m_chatGui.handleEvent(evt);
	}
	else if (evt.type == sf::Event::KeyPressed) {
		if (evt.key.code == sf::Keyboard::Escape)
			setState(MenuState::OPEN);
		else if (evt.key.code == sf::Keyboard::Return)
		{
			setState(MenuState::CHAT);
			tgui::Widget::Ptr chatEdit = m_chatGui.get("ChatEdit");
			m_chatGui.focusWidget(chatEdit);
		}
	}
}

void Menu::receive(const ChatEvent& evt)
{
	tgui::ChatBox::Ptr chatBox = m_chatGui.get("ChatBox");
	chatBox->addLine(evt.message);
}