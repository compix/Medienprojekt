#include "Menu.h"
#include "MenuPage.h"

#include "../GameGlobals.h"



Menu::Menu()
	: m_gui(*GameGlobals::window), m_loadingPage(*this), m_rootPage(*this), m_chatPage(*this), m_lobbyPage(*this)
{
	m_preloadConnection = GameGlobals::events->preload.connect(this, &Menu::onPreload);
	GameGlobals::events->forceDisconnect.connect(this, &Menu::onForceDisconnect);

	pushPage(&m_loadingPage);
}

void Menu::draw()
{
	if (!m_pageStack.empty())
		m_gui.draw(false);
}

void Menu::onSfml(const sf::Event &evt)
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

void Menu::onPreload(int progress, int total, string nextSection, const string &nextFilename)
{
	if (progress == total)
	{
		popPage();
		m_preloadConnection.disconnect();
		GameGlobals::events->sfml.connect(this, &Menu::onSfml);
	}
}

void Menu::onForceDisconnect(const string &message)
{
	if (!message.empty()) {
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
		GameGlobals::events->menuShow.emit(true);
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
			GameGlobals::events->menuShow.emit(false);
	}
}

void Menu::popAllPages()
{
	while (!m_pageStack.empty())
		popPage();
}