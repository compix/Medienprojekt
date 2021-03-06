#include "MenuPageChat.h"
#include "../../Events/SendChatEvent.h"
#include "../../GameGlobals.h"
#include "../../Events/DisconnectEvent.h"
#include "../../Network/NetPlayerInfo.h"

MenuPageChat::MenuPageChat(Menu &menu)
	:MenuPage(menu)
{
	GameGlobals::events->subscribe<ChatEvent>(*this);
	GameGlobals::events->subscribe<PlayerJoinEvent>(*this);
	GameGlobals::events->subscribe<DisconnectEvent>(*this);

	m_chatBox = createChatBox(20, 20, 760, 500);

	m_editBox = createEditBox(20, 540, 760, 40);
	m_editBox->bindCallback(&MenuPageChat::onSubmit, this, tgui::EditBox::ReturnKeyPressed);

	m_onShowFocus = m_editBox.get();
}

void MenuPageChat::onSubmit()
{
	sf::String message = m_editBox->getText();
	if (!message.isEmpty())
	{
		GameGlobals::events->emit<SendChatEvent>(message);
		m_editBox->setText("");
	}
}

void MenuPageChat::receive(const ChatEvent &evt)
{
	m_chatBox->addLine(evt.name + ": " + evt.message);
}

void MenuPageChat::receive(const PlayerJoinEvent &evt)
{
	m_chatBox->addLine(">" + evt.name + " joined the game", sf::Color::Green);
}

void MenuPageChat::receive(const DisconnectEvent& evt)
{
	if (evt.playerInfo)
		m_chatBox->addLine(">" + evt.playerInfo->name + " left the game", sf::Color::Green);
}
