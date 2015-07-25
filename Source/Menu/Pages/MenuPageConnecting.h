#pragma once
#include "../MenuPage.h"
#include "MenuPageLobby.h"
#include "../../Events/ChatEvent.h"
#include "../../Events/PlayerJoinEvent.h"

struct ClientStateEvent;
using entityx::Receiver;

class MenuPageConnecting : public MenuPage, public Receiver<MenuPageConnecting>
{
public:
	MenuPageConnecting(Menu &menu);

	void show() override;
	void hide() override;
	virtual void onEscape() override {}

	void receive(const ClientStateEvent& evt);

protected:
	void onAbort();
	tgui::ChatBox::Ptr m_chatBox;
};
