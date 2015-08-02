#include "MenuPageChat.h"
#include "../../GameGlobals.h"

#include "../../Network/NetPlayerInfo.h"

MenuPageChat::MenuPageChat(Menu &menu)
	:MenuPage(menu)
{
	GameGlobals::events->chat.connect(this, MenuPageChat::onChat);
	GameGlobals::events->playerJoin.connect(this, MenuPageChat::onPlayerJoin);
	GameGlobals::events->disconnect.connect(this, MenuPageChat::onDisconnect);

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
		GameGlobals::events->sendChat.emit(message);
		m_editBox->setText("");
	}
}

void MenuPageChat::onChat(const string &message, const string &name)
{
	m_chatBox->addLine(name + ": " + message);
}

void MenuPageChat::onPlayerJoin(uint8_t playerIndex, const string &name)
{
	m_chatBox->addLine(">" + name + " joined the game", sf::Color::Green);
}

void MenuPageChat::onDisconnect(const string &reason, NetPlayerInfo *playerInfo)
{
	if (playerInfo)
		m_chatBox->addLine(">" + playerInfo->name + " left the game", sf::Color::Green);
}
