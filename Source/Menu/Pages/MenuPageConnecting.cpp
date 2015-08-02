#include "MenuPageConnecting.h"

#include "../../GameGlobals.h"


#include "../Menu.h"

MenuPageConnecting::MenuPageConnecting(Menu &menu)
	:MenuPage(menu)
{
	m_chatBox = createChatBox(20, 20, 760, 500);
	auto button = createButton(20, 540, 760, 40, "Abort");
	button->bindCallback(&MenuPageConnecting::onAbort, this, tgui::Button::LeftMouseClicked);

	m_onShowFocus = button.get();
	m_clientStateConnection = GameGlobals::events->clientState.connect(this, MenuPageConnecting::onClientState);
	m_clientStateConnection.disable();
}

void MenuPageConnecting::show()
{
	MenuPage::show();

	m_chatBox->removeAllLines();
	m_clientStateConnection.enable();
}

void MenuPageConnecting::hide()
{
	m_clientStateConnection.disable();

	MenuPage::hide();
}

void MenuPageConnecting::onAbort()
{
	GameGlobals::events->forceDisconnect.emit("");
	close();
}

void MenuPageConnecting::onClientState(const string &message, ClientState state)
{
	m_chatBox->addLine(message);
	if (state == ClientState::PREGAME)
		m_menu.popAllPages();
	else if (state == ClientState::LOBBY)
	{
		m_menu.popPage();
		m_menu.showLobby();
	}
}
