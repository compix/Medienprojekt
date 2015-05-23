#pragma once
#include "../MenuPage.h"

class MenuPageJoinServer : public MenuPage
{
public:
	MenuPageJoinServer(Menu &menu);
	void onSubmit();
private:
	tgui::EditBox::Ptr m_name;
	tgui::EditBox::Ptr m_host;
	tgui::EditBox::Ptr m_port;
};
