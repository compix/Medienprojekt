#include "MenuPageLobby.h"
#include "../../GameGlobals.h"
#include "MenuPageChat.h"
#include "../../Events/SendChatEvent.h"
#include "../../Events/LobbyEvent.h"
#include <format.h>
#include "../../Events/SetReadyEvent.h"
#include "../../Events/ReadyEvent.h"
#include "../../Events/StartGameEvent.h"
#include "../Menu.h"

MenuPageLobby::MenuPageLobby(Menu &menu)
	:MenuPage(menu)
{
	createPicture(800, 600, "Assets/ui/xubuntu_bg_aluminium.jpg");
	createLabel(50, 50, "Lobby");

	auto readySize = 40;
	auto x1 = 50;
	auto x2 = 210;
	auto y = 120;
	auto stepY = 50;
	auto labelYOffset = 6;

	for (int i = 0; i < GameConstants::MAX_PLAYERS; i++)
	{
		m_name[i] = createLabel(x1, y + labelYOffset, fmt::format("Player {}:", i + 1));
		m_ready[i] = createCheckbox(x2, y, readySize, readySize);
		m_ready[i]->bindCallback(std::bind(&MenuPageLobby::onChange, this, i), tgui::Checkbox::Checked | tgui::Checkbox::Unchecked);
		y += stepY;
	}

	m_timer = createLabel(x1, 540 + labelYOffset, "");

	m_chatBox = createChatBox(320, 20, 460, 500);

	m_editBox = createEditBox(320, 540, 460, 40);
	m_editBox->bindCallback(&MenuPageLobby::onSubmit, this, tgui::EditBox::ReturnKeyPressed);

	m_onShowFocus = m_editBox.get();
}

void MenuPageLobby::show()
{
	MenuPage::show();

	m_chatBox->removeAllLines();


	for (int i = 0; i < GameConstants::MAX_PLAYERS; i++)
	{
		m_name[i]->setText(fmt::format("Player {}:", i + 1));
		m_ready[i]->uncheck();
	}

	m_timer->setText("Starting in: 15");
	GameGlobals::events->subscribe<ChatEvent>(*this);
	GameGlobals::events->subscribe<PlayerJoinEvent>(*this);
	GameGlobals::events->subscribe<LobbyEvent>(*this);
	GameGlobals::events->subscribe<StartGameEvent>(*this);
	GameGlobals::events->subscribe<ReadyEvent>(*this);
}

void MenuPageLobby::hide()
{
	GameGlobals::events->unsubscribe<ChatEvent>(*this);
	GameGlobals::events->unsubscribe<PlayerJoinEvent>(*this);
	GameGlobals::events->unsubscribe<LobbyEvent>(*this);
	GameGlobals::events->unsubscribe<StartGameEvent>(*this);
	GameGlobals::events->unsubscribe<ReadyEvent>(*this);

	MenuPage::hide();
}

void MenuPageLobby::onSubmit()
{
	sf::String message = m_editBox->getText();
	if (!message.isEmpty())
	{
		GameGlobals::events->emit<SendChatEvent>(message);
		m_editBox->setText("");
	}
}

void MenuPageLobby::onChange(int index)
{
	GameGlobals::events->emit<SetReadyEvent>(index, m_ready[index]->isChecked());
	
}

void MenuPageLobby::receive(const LobbyEvent& evt)
{
	for (int i = 0; i < GameConstants::MAX_PLAYERS; i++)
	{
		m_name[i]->setText(evt.name[i]);
		setEnabled(m_ready[i], evt.enabled[i]);
		setChecked(m_ready[i], evt.ready[i]);
		setVisible(m_name[i], i < evt.numPlayers);
		setVisible(m_ready[i], i < evt.numPlayers);
	}
}

void MenuPageLobby::receive(const ChatEvent &evt)
{
	m_chatBox->addLine(evt.name + ": " + evt.message);
}

void MenuPageLobby::receive(const PlayerJoinEvent &evt)
{
	m_name[evt.playerIndex]->setText(evt.name);
	setChecked(m_ready[evt.playerIndex], false);
	m_chatBox->addLine(">" + evt.name + " joined the game", sf::Color::Green);
}

void MenuPageLobby::receive(const ReadyEvent& evt)
{
	setChecked(m_ready[evt.playerIndex], evt.ready);
}

void MenuPageLobby::receive(const StartGameEvent& evt)
{
	m_menu.popAllPages();
}
