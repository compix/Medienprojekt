#pragma once
#include <string>
#include <TGUI/TGUI.hpp>
#include <entityx/entityx.h>

using std::string;
class Menu;

class MenuPage
{
public:
	MenuPage(Menu &menu);
	virtual ~MenuPage();
	virtual void hide() { m_panel->hide(); }

	virtual void show();
	virtual void close();
	virtual void onEscape() { close(); }

protected:
	tgui::Picture::Ptr createPicture(float width, float height, const string &filename);
	tgui::Label::Ptr createLabel(float x, float y, const string &text);
	tgui::EditBox::Ptr createEditBox(float x, float y, float width, float height);
	tgui::Checkbox::Ptr createCheckbox(float x, float y, float width, float height);
	tgui::Button::Ptr createButton(float x, float y, float width, float height, const string &text);
	tgui::ChatBox::Ptr createChatBox(float x, float y, float width, float height);
	tgui::ComboBox::Ptr createComboBox(float x, float y, float width, float height);

protected:
	Menu &m_menu;
	tgui::Gui &m_gui;
	tgui::Panel::Ptr m_panel;
	tgui::Widget *m_onShowFocus;
};
