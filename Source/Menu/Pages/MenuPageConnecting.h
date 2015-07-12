#pragma once
#include "../MenuPage.h"
#include "../../Events/ChatEvent.h"
#include "../../Events/PlayerJoinEvent.h"

struct ConnectionStateEvent;
using entityx::Receiver;

class MenuPageConnecting : public MenuPage, public Receiver<MenuPageConnecting>
{
public:
	MenuPageConnecting(Menu &menu);

	void show() override;
	void hide() override;

	void receive(const ConnectionStateEvent& evt);
	virtual void onEscape() override {}

protected:
	void onAbort();
	tgui::ChatBox::Ptr m_chatBox;
};
