#pragma once
#include "../MenuPage.h"
#include "../../GameConstants.h"

struct ReadyEvent;
struct StartGameEvent;
struct LobbyEvent;
struct ChatEvent;
struct PlayerJoinEvent;
using entityx::Receiver;

class MenuPageLobby : public MenuPage, public Receiver<MenuPageLobby>
{
public:
	MenuPageLobby(Menu &menu);

	void show() override;
	void hide() override;
	virtual void onEscape() override {}

	void receive(const LobbyEvent& evt);
	void receive(const ChatEvent& evt);
	void receive(const PlayerJoinEvent& evt);
	void receive(const ReadyEvent& evt);
	void receive(const StartGameEvent& evt);

	void onChange(int index);

private:
	void onSubmit();

private:
	tgui::Label::Ptr m_name[GameConstants::MAX_PLAYERS];
	tgui::Checkbox::Ptr m_ready[GameConstants::MAX_PLAYERS];
	tgui::ChatBox::Ptr m_chatBox;
	tgui::EditBox::Ptr m_editBox;
	tgui::Label::Ptr m_timer;
};
