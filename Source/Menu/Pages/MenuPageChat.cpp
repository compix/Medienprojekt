#include "MenuPageChat.h"
#include "../../Events/SendChatEvent.h"
#include "../../GameGlobals.h"

MenuPageChat::MenuPageChat(Menu &menu)
	:MenuPage(menu)
{
	GameGlobals::events->subscribe<ChatEvent>(*this);
	m_chatBox = createChatBox(20, 20, 760, 500);

	m_editBox = createEditBox(20, 540, 760, 40);
	m_editBox->bindCallback(&MenuPageChat::onSubmit, this, tgui::EditBox::ReturnKeyPressed);

	m_onShowFocus = m_editBox.get();
}

void MenuPageChat::onSubmit()
{
	sf::String message = m_editBox->getText();
	GameGlobals::events->emit<SendChatEvent>(message);
	m_editBox->setText("");
}

void MenuPageChat::receive(const ChatEvent &evt)
{
	m_chatBox->addLine(evt.name + ": " + evt.message);
}
