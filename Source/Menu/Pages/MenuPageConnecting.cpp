#include "MenuPageConnecting.h"
#include "../../Events/SendChatEvent.h"
#include "../../GameGlobals.h"
#include "../../Events/ConnectionStateEvent.h"
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
	GameGlobals::events->subscribe<ConnectionStateEvent>(*this);
}

void MenuPageConnecting::hide()
{
	GameGlobals::events->unsubscribe<ConnectionStateEvent>(*this);

	MenuPage::hide();
}

void MenuPageConnecting::onAbort()
{
	GameGlobals::events->emit<ForceDisconnectEvent>();
	close();
}

void MenuPageConnecting::receive(const ConnectionStateEvent &evt)
{
	m_chatBox->addLine(evt.message);
	if (evt.finished)
		m_menu.popAllPages(); //fixme: if game not started, pop one, push lobby
}
