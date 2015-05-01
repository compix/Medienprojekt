#include "Menu.h"

Menu::Menu(sf::RenderWindow &window) : m_gui(window)
{
	// Load the font (you should check the return value to make sure that it is loaded)
	m_gui.setGlobalFont("Assets/fonts/DejaVuSans.ttf");

	// Load the widgets
	loadWidgets();
}

Menu::~Menu() { }

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
}

void Menu::loginClicked()
{
	// Get the username and password
	tgui::EditBox::Ptr editBoxUsername = m_gui.get("Username");
	tgui::EditBox::Ptr editBoxPassword = m_gui.get("Password");

	sf::String username = editBoxUsername->getText();
	sf::String password = editBoxPassword->getText();
	m_visible = false;
}

bool Menu::handleEvent(const sf::Event &evt)
{
	if (m_visible)
		m_gui.handleEvent(evt);
	return m_visible;
}

void Menu::draw()
{
	if (m_visible)
		m_gui.draw();
}