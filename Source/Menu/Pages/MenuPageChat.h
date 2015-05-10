#pragma once
#include "../MenuPage.h"
#include "../../Events/ChatEvent.h"

using entityx::Receiver;

class MenuPageChat : public MenuPage, public Receiver<MenuPageChat>
{
public:
	MenuPageChat(Menu &menu);

	void receive(const ChatEvent& evt);
	void show() override;

protected:
	void onSubmit();
protected:
	tgui::ChatBox::Ptr m_chatBox;
	tgui::EditBox::Ptr m_editBox;
};
