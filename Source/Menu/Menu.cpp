#include "Menu.h"
#include "MenuPage.h"
#include "../Events/MenuShowEvent.h"

Menu::Menu(sf::RenderWindow &window, EventManager &events)
	: m_gui(window), m_events(events), m_rootPage(*this), m_chatPage(*this)
{
	events.subscribe<sf::Event>(*this);
}

void Menu::draw()
{
	if (!m_pageStack.empty())
		m_gui.draw();
}

void Menu::receive(const sf::Event &evt)
{
	if (!m_pageStack.empty())
	{
		if (evt.type == sf::Event::KeyPressed && evt.key.code == sf::Keyboard::Escape)
			m_pageStack.top()->onEscape();
		else
			m_gui.handleEvent(evt);
	}
	else if (evt.type == sf::Event::KeyPressed)
	{
		if (evt.key.code == sf::Keyboard::Escape)
			pushPage(&m_rootPage);
		else if (evt.key.code == sf::Keyboard::Return)
			pushPage(&m_chatPage);
	}
}

void Menu::pushPage(MenuPage *page)
{
	if (!m_pageStack.empty())
		m_pageStack.top()->hide();

	m_pageStack.push(page);
	page->show();

	if (m_pageStack.size() == 1)
		m_events.emit<MenuShowEvent>(true);
}

void Menu::popPage()
{
	if (!m_pageStack.empty())
	{
		m_pageStack.top()->hide();
		m_pageStack.pop();

		if (!m_pageStack.empty())
			m_pageStack.top()->show();
		else
			m_events.emit<MenuShowEvent>(false);
	}
}
