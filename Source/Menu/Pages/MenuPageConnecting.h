#pragma once
#include "../MenuPage.h"
#include "MenuPageLobby.h"
#include <signal11/Signal.h>

enum class ClientState;

class MenuPageConnecting : public MenuPage
{
public:
	MenuPageConnecting(Menu &menu);

	void show() override;
	void hide() override;
	virtual void onEscape() override {}

private:
	void onClientState(const string &message, ClientState state);

protected:
	void onAbort();
	tgui::ChatBox::Ptr m_chatBox;
	ScopedConnectionRef m_clientStateConnection;
};
