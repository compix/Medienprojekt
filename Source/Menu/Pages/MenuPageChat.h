#pragma once
#include "../MenuPage.h"
#include <stdint.h>
struct NetPlayerInfo;

class MenuPageChat : public MenuPage
{
public:
	MenuPageChat(Menu &menu);
	
private:
	void onChat(const string &message, const string &name);
	void onPlayerJoin(uint8_t playerIndex, const string &name);
	void onDisconnect(const string &reason, NetPlayerInfo *playerInfo);
protected:
	void onSubmit();
protected:
	tgui::ChatBox::Ptr m_chatBox;
	tgui::EditBox::Ptr m_editBox;
};
