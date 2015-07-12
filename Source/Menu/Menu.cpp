#include "Menu.h"
#include "MenuPage.h"
#include "../Events/MenuShowEvent.h"
#include "../GameGlobals.h"

Menu::Menu()
	: m_gui(*GameGlobals::window), m_rootPage(*this), m_chatPage(*this)
{
	GameGlobals::events->subscribe<sf::Event>(*this);
//	pushPage(&m_rootPage);
}

void Menu::draw()
{
	if (!m_pageStack.empty())
		m_gui.draw(false);
}

void Menu::receive(const sf::Event &evt)
{
	if (!m_pageStack.empty())
	{
		if (evt.type == sf::Event::KeyPressed && evt.key.code == sf::Keyboard::Escape)
			m_pageStack.top()->onEscape();
		else
			m_gui.handleEvent(evt, false);
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
		GameGlobals::events->emit<MenuShowEvent>(true);
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
			GameGlobals::events->emit<MenuShowEvent>(false);
	}
}

void Menu::popAllPages()
{
	while (!m_pageStack.empty())
		popPage();
}