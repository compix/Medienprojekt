#include "Menu.h"
#include "MenuPage.h"
#include "../Events/MenuShowEvent.h"
#include "../GameGlobals.h"
#include "../Events/PreloadEvent.h"
#include "../Events/ForceDisconnectEvent.h"

Menu::Menu()
	: m_gui(*GameGlobals::window), m_loadingPage(*this), m_rootPage(*this), m_chatPage(*this), m_lobbyPage(*this)
{
	GameGlobals::events->subscribe<PreloadEvent>(*this);
	GameGlobals::events->subscribe<ForceDisconnectEvent>(*this);

	pushPage(&m_loadingPage);
}

void Menu::draw()
{
	if (!m_pageStack.empty())
		m_gui.draw(false);
	if (!m_pageStack.empty())
		m_pageStack.top()->update();
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

void Menu::receive(const PreloadEvent& evt)
{
	if (evt.progress == evt.total)
	{
		popPage();
		pushPage(&m_rootPage);
		GameGlobals::events->unsubscribe<PreloadEvent>(*this);
		GameGlobals::events->subscribe<sf::Event>(*this);
	}
}

void Menu::receive(const ForceDisconnectEvent& evt)
{
	if (!evt.message.empty()) {
		popAllPages();
		pushPage(&m_rootPage);
		//fixme: push error message page
	}
}

void Menu::showLobby()
{
	pushPage(&m_lobbyPage);
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