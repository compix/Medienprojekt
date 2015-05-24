#pragma once
#include "../MenuPage.h"

class MenuPageLocalGame : public MenuPage
{
public:
	MenuPageLocalGame(Menu &menu);
	void onSubmit();
private:
	tgui::EditBox::Ptr m_name[4];
	tgui::EditBox::Ptr m_width;
	tgui::EditBox::Ptr m_height;
};
