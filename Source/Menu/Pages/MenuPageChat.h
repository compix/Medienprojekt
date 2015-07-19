#pragma once
#include "../MenuPage.h"
#include "../../Events/ChatEvent.h"
#include "../../Events/PlayerJoinEvent.h"
#include "../../Events/DisconnectEvent.h"

using entityx::Receiver;

class MenuPageChat : public MenuPage, public Receiver<MenuPageChat>
{
public:
	MenuPageChat(Menu &menu);

	void receive(const ChatEvent& evt);
	void receive(const PlayerJoinEvent& evt);
	void receive(const DisconnectEvent& evt);
protected:
	void onSubmit();
protected:
	tgui::ChatBox::Ptr m_chatBox;
	tgui::EditBox::Ptr m_editBox;
};
