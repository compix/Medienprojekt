#pragma once
#include "../MenuPage.h"
#include "../../GameConstants.h"
#include <signal11/Signal.h>

struct NetPlayerInfo;
enum class CreateGamePlayerType;
struct LobbyEvent;

class MenuPageLobby : public MenuPage
{
public:
	MenuPageLobby(Menu &menu);

	void show() override;
	void hide() override;
	virtual void onEscape() override {}

	void onChange(int index);

private:
	void onSubmit();
	void onAbort();

	void onLobby(const LobbyEvent& evt);
	void onLobbyDisable();
	void onChat(const string &message, const string &name);
	void onPlayerJoin(uint8_t playerIndex, const string &name);
	void onDisconnect(const string &reason, NetPlayerInfo *playerInfo);
	void onReady(uint8_t playerIndex, bool ready);
	void onStartGame();
	void onCountdown(const string &message);

private:
	tgui::Label::Ptr m_name[GameConstants::MAX_PLAYERS];
	tgui::Checkbox::Ptr m_ready[GameConstants::MAX_PLAYERS];
	tgui::ChatBox::Ptr m_chatBox;
	tgui::EditBox::Ptr m_editBox;
	tgui::Label::Ptr m_timer;
	bool m_ignoreChecked = false;
	ConnectionScope m_connections;
};
