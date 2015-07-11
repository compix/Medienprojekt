#pragma once
#include "../MenuPage.h"

class MenuPageJoinGame : public MenuPage
{
public:
	MenuPageJoinGame(Menu &menu);
	void onSubmit();
private:
	tgui::EditBox::Ptr m_name;
	tgui::EditBox::Ptr m_host;
	tgui::EditBox::Ptr m_port;
};
