#pragma once
#include <string>
#include <TGUI/TGUI.hpp>
#include <entityx/entityx.h>

using std::string;
class Menu;

class Label
{
public:
	Label(tgui::Panel::Ptr &panel) : m_shadow(*panel), m_label(*panel)
	{
		m_shadow->setTextColor(sf::Color::Black);
	}

	void setText(const string &text)
	{
		m_shadow->setText(text);
		m_label->setText(text);
	}

	void setPosition(float x, float y, float shadowOffset)
	{
		m_shadow->setPosition(x + shadowOffset, y + shadowOffset);
		m_label->setPosition(x, y);
	}

	tgui::Label::Ptr getShadow()
	{
		return m_shadow;
	}

	tgui::Label::Ptr getLabel()
	{
		return m_label;
	}

	void setTextStyle(sf::Uint32 style)
	{
		m_shadow->setTextStyle(style);
		m_label->setTextStyle(style);
	}

	void setTextSize(unsigned int size)
	{
		m_shadow->setTextSize(size);
		m_label->setTextSize(size);
	}
protected:
	tgui::Label::Ptr m_shadow;
	tgui::Label::Ptr m_label;
};

class MenuPage
{
public:
	MenuPage(Menu &menu);
	virtual ~MenuPage();

	virtual void show();
	virtual void hide() { m_panel->hide(); }
	virtual void close();
	virtual void onEscape() { close(); }

protected:
	tgui::Picture::Ptr createPicture(float width, float height, const string &filename);
	std::shared_ptr<Label> createLabel(float x, float y, const string &text, float shadowOffset=1);
	tgui::EditBox::Ptr createEditBox(float x, float y, float width, float height);
	tgui::Checkbox::Ptr createCheckbox(float x, float y, float width, float height);
	tgui::Button::Ptr createButton(float x, float y, float width, float height, const string &text);
	tgui::ChatBox::Ptr createChatBox(float x, float y, float width, float height);
	tgui::ComboBox::Ptr createComboBox(float x, float y, float width, float height);
	tgui::LoadingBar::Ptr createLoadingBar(float x, float y, float width, float height, int min, int max);
	void setVisible(tgui::Widget::Ptr widget, const bool visible);
	void setEnabled(tgui::Widget::Ptr widget, bool enable);
	void setEnabledComboBox(tgui::ComboBox::Ptr widget, bool enable);
	void setChecked(tgui::Checkbox::Ptr widget, const bool checked);

protected:
	Menu &m_menu;
	tgui::Gui &m_gui;
	tgui::Panel::Ptr m_panel;
	tgui::Widget *m_onShowFocus;
};
