#pragma once
#include "../MenuPage.h"

class MenuPageCreateServer : public MenuPage
{
public:
	MenuPageCreateServer(Menu &menu);
	void onSubmit();
private:
	tgui::EditBox::Ptr m_name;
	tgui::EditBox::Ptr m_host;
	tgui::EditBox::Ptr m_port;
	tgui::EditBox::Ptr m_width;
	tgui::EditBox::Ptr m_height;
};
