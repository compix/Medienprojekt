#include "MenuPageLobby.h"
#include "../../GameGlobals.h"
#include "MenuPageChat.h"
#include "../../Events/SendChatEvent.h"
#include "../../Events/LobbyEvent.h"
#include "../../Events/CountdownEvent.h"
#include <format.h>
#include "../../Events/SetReadyEvent.h"
#include "../../Events/ReadyEvent.h"
#include "../../Events/StartGameEvent.h"
#include "../Menu.h"
#include "../../Events/ForceDisconnectEvent.h"
#include "../../Events/DisconnectEvent.h"
#include "../../Network/NetPlayerInfo.h"

MenuPageLobby::MenuPageLobby(Menu &menu)
	:MenuPage(menu)
{
	createPicture(800, 600, "Assets/ui/xubuntu_bg_aluminium.jpg");
	createLabel(50, 50, "Lobby");

	auto readySize = 40.0f;
	auto x1 = 50.0f;
	auto x2 = 210.0f;
	auto y = 120.0f;
	auto stepY = 50.0f;
	auto labelYOffset = 6.0f;

	for (int i = 0; i < GameConstants::MAX_PLAYERS; i++)
	{
		m_name[i] = createLabel(x1, y + labelYOffset, fmt::format("Player {}:", i + 1));
		m_ready[i] = createCheckbox(x2, y, readySize, readySize);
		m_ready[i]->bindCallback(std::bind(&MenuPageLobby::onChange, this, i), tgui::Checkbox::Checked | tgui::Checkbox::Unchecked);
		y += stepY;
	}

	m_timer = createLabel(x1, 480 + labelYOffset, "");

	m_chatBox = createChatBox(320, 20, 460, 500);

	m_editBox = createEditBox(320, 540, 460, 40);
	m_editBox->bindCallback(&MenuPageLobby::onSubmit, this, tgui::EditBox::ReturnKeyPressed);

	auto button = createButton(x1, 540, (x2 - x1 + readySize), 40, "Disconnect");
	button->bindCallback(&MenuPageLobby::onAbort, this, tgui::Button::LeftMouseClicked);

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
	GameGlobals::events->subscribe<LobbyEventDisable>(*this);
	GameGlobals::events->subscribe<StartGameEvent>(*this);
	GameGlobals::events->subscribe<ReadyEvent>(*this);
	GameGlobals::events->subscribe<CountdownEvent>(*this);
	GameGlobals::events->subscribe<DisconnectEvent>(*this);
}

void MenuPageLobby::hide()
{
	GameGlobals::events->unsubscribe<ChatEvent>(*this);
	GameGlobals::events->unsubscribe<PlayerJoinEvent>(*this);
	GameGlobals::events->unsubscribe<LobbyEvent>(*this);
	GameGlobals::events->unsubscribe<LobbyEventDisable>(*this);
	GameGlobals::events->unsubscribe<StartGameEvent>(*this);
	GameGlobals::events->unsubscribe<ReadyEvent>(*this);
	GameGlobals::events->unsubscribe<CountdownEvent>(*this);
	GameGlobals::events->unsubscribe<DisconnectEvent>(*this);

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

void MenuPageLobby::onAbort()
{
	GameGlobals::events->emit<ForceDisconnectEvent>();
	m_menu.popPage();
}

void MenuPageLobby::onChange(int index)
{
	if(!m_ignoreChecked)
		GameGlobals::events->emit<SetReadyEvent>(index, m_ready[index]->isChecked());
}

void MenuPageLobby::receive(const LobbyEvent& evt)
{
	m_ignoreChecked = true;
	for (int i = 0; i < GameConstants::MAX_PLAYERS; i++)
	{
		m_name[i]->setText(evt.name[i]);
		setEnabled(m_ready[i], evt.enabled[i]);
		setChecked(m_ready[i], evt.ready[i]);
		setVisible(m_name[i], i < evt.numPlayers);
		setVisible(m_ready[i], i < evt.numPlayers);
	}
	m_ignoreChecked = false;
}

void MenuPageLobby::receive(const LobbyEventDisable& evt)
{
	for (int i = 0; i < GameConstants::MAX_PLAYERS; i++)
		setEnabled(m_ready[i], false);
}


void MenuPageLobby::receive(const ChatEvent &evt)
{
	m_chatBox->addLine(evt.name + ": " + evt.message);
}

void MenuPageLobby::receive(const PlayerJoinEvent &evt)
{
	m_ignoreChecked = true;
	m_name[evt.playerIndex]->setText(evt.name);
	setChecked(m_ready[evt.playerIndex], false);
	m_chatBox->addLine(">" + evt.name + " joined the game", sf::Color::Green);
	m_ignoreChecked = false;
}

void MenuPageLobby::receive(const DisconnectEvent& evt)
{
	if (evt.playerInfo)
	{
		m_ignoreChecked = true;
		m_name[evt.playerInfo->playerIndex]->setText("?");
		setChecked(m_ready[evt.playerInfo->playerIndex], false);
		m_chatBox->addLine(">" + evt.playerInfo->name + " left the game", sf::Color::Green);
		m_ignoreChecked = false;
	}
}

void MenuPageLobby::receive(const ReadyEvent& evt)
{
	m_ignoreChecked = true;
	setChecked(m_ready[evt.playerIndex], evt.ready);
	m_ignoreChecked = false;
}

void MenuPageLobby::receive(const StartGameEvent& evt)
{
	m_menu.popAllPages();
}

void MenuPageLobby::receive(const CountdownEvent& evt)
{
	m_timer->setText(evt.message);
}