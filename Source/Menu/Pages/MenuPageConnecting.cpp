#include "MenuPageConnecting.h"
#include "../../Events/SendChatEvent.h"
#include "../../GameGlobals.h"
#include "../../Events/ClientStateEvent.h"
#include "../../Events/ForceDisconnectEvent.h"
#include "../Menu.h"

MenuPageConnecting::MenuPageConnecting(Menu &menu)
	:MenuPage(menu)
{
	m_chatBox = createChatBox(20, 20, 760, 500);
	auto button = createButton(20, 540, 760, 40, "Abort");
	button->bindCallback(&MenuPageConnecting::onAbort, this, tgui::Button::LeftMouseClicked);

	m_onShowFocus = button.get();
}

void MenuPageConnecting::show()
{
	MenuPage::show();

	m_chatBox->removeAllLines();
	GameGlobals::events->subscribe<ClientStateEvent>(*this);
}

void MenuPageConnecting::hide()
{
	GameGlobals::events->unsubscribe<ClientStateEvent>(*this);

	MenuPage::hide();
}

void MenuPageConnecting::onAbort()
{
	GameGlobals::events->emit<ForceDisconnectEvent>();
	close();
}

void MenuPageConnecting::receive(const ClientStateEvent &evt)
{
	m_chatBox->addLine(evt.message);
	if (evt.state == ClientState::PREGAME)
		m_menu.popAllPages();
	else if (evt.state == ClientState::LOBBY)
	{
		m_menu.popPage();
		m_menu.showLobby();
	}
}
