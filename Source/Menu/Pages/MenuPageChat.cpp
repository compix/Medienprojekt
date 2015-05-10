#include "MenuPageChat.h"
#include "../../Events/SendChatEvent.h"

MenuPageChat::MenuPageChat(Menu &menu)
	:MenuPage(menu)
{
	m_events.subscribe<ChatEvent>(*this);
	m_chatBox = createChatBox(20, 20, 760, 500);

	m_editBox = createEditBox(20, 540, 760, 40);
	m_editBox->bindCallback(&MenuPageChat::onSubmit, this, tgui::EditBox::ReturnKeyPressed);
}

void MenuPageChat::onSubmit()
{
	sf::String message = m_editBox->getText();
	m_events.emit<SendChatEvent>(message);
	m_editBox->setText("");
}

void MenuPageChat::receive(const ChatEvent &evt)
{
	m_chatBox->addLine(evt.message);
}

void MenuPageChat::show()
{
	MenuPage::show();
	m_editBox->focus();
}
