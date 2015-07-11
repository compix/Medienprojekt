#pragma once
#include "../MenuPage.h"

class MenuPageCreateGame : public MenuPage
{
public:
	MenuPageCreateGame(Menu &menu);
	void onSubmit();
	void onChange();
	void setEnabled(tgui::Widget::Ptr widget, bool enable);
	void setEnabledComboBox(tgui::ComboBox::Ptr widget, bool enable);
	tgui::ComboBox::Ptr createSizeComboBox(float x, float y, float width, float height, int from, int to, int selected);

private:
	tgui::EditBox::Ptr m_name[4];
	tgui::ComboBox::Ptr m_type[4];
	tgui::ComboBox::Ptr m_playerCount;
	tgui::ComboBox::Ptr m_width;
	tgui::ComboBox::Ptr m_height;
	tgui::EditBox::Ptr m_host;
	tgui::EditBox::Ptr m_port;
	tgui::Button::Ptr m_submitButton;
};
