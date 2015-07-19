#include "MenuPage.h"
#include "Menu.h"

MenuPage::MenuPage(Menu &menu)
	:m_menu(menu), m_gui(menu.m_gui), m_panel(m_gui), m_onShowFocus(nullptr)
{
	m_panel->setSize(800, 600);
	m_panel->setBackgroundColor(sf::Color::Transparent);
	m_panel->hide();
}


MenuPage::~MenuPage() {}

void MenuPage::show()
{
	m_panel->show();

	if (m_onShowFocus)
		m_onShowFocus->focus();
}

void MenuPage::close()
{
	m_menu.popPage();
}

tgui::Picture::Ptr MenuPage::createPicture(float width, float height, const string &filename)
{
	tgui::Picture::Ptr picture(*m_panel);
	picture->load(filename);
	picture->setSize(width, height);
	return picture;
}

tgui::Label::Ptr MenuPage::createLabel(float x, float y, const string &text)
{
	tgui::Label::Ptr label(*m_panel);
	label->setText(text);
	label->setPosition(x, y);
	return label;
}

tgui::EditBox::Ptr MenuPage::createEditBox(float x, float y, float width, float height)
{
	tgui::EditBox::Ptr editbox(*m_panel);
	editbox->load("Assets/ui/widgets/Black.conf");
	editbox->setPosition(x, y);
	editbox->setSize(width, height);
	return editbox;
}

tgui::Checkbox::Ptr MenuPage::createCheckbox(float x, float y, float width, float height)
{
	tgui::Checkbox::Ptr checkbox(*m_panel);
	checkbox->load("Assets/ui/widgets/Black.conf");
	checkbox->setPosition(x, y);
	checkbox->setSize(width, height);
	return checkbox;
}

tgui::Button::Ptr MenuPage::createButton(float x, float y, float width, float height, const string &text)
{
	tgui::Button::Ptr button(*m_panel);
	button->load("Assets/ui/widgets/Black.conf");
	button->setPosition(x, y);
	button->setSize(width, height);
	button->setTextSize(18);
	button->setText(text);
	return button;
}

tgui::ChatBox::Ptr MenuPage::createChatBox(float x, float y, float width, float height)
{
	tgui::ChatBox::Ptr chatBox(*m_panel);
	chatBox->load("Assets/ui/widgets/Black.conf");
	chatBox->setPosition(x, y);
	chatBox->setSize(width, height);
	chatBox->setTextColor(sf::Color::White);
	return chatBox;
}

tgui::ComboBox::Ptr MenuPage::createComboBox(float x, float y, float width, float height)
{
	tgui::ComboBox::Ptr comboBox(*m_panel);
	comboBox->load("Assets/ui/widgets/Black.conf");
	comboBox->setPosition(x, y);
	comboBox->setSize(width, height);
	return comboBox;
}

tgui::LoadingBar::Ptr MenuPage::createLoadingBar(float x, float y, float width, float height, int min, int max)
{
	tgui::LoadingBar::Ptr loadingBar(*m_panel);
	loadingBar->load("Assets/ui/widgets/Black.conf");
	loadingBar->setPosition(x, y);
	loadingBar->setSize(width, height);
	loadingBar->setMinimum(min);
	loadingBar->setMaximum(max);
	return loadingBar;
}

void MenuPage::setVisible(tgui::Widget::Ptr widget, const bool visible)
{
	if (visible)
		widget->show();
	else
		widget->hide();
}

void MenuPage::setEnabled(tgui::Widget::Ptr widget, bool enable)
{
	if (enable)
	{
		widget->enable();
		widget->setTransparency(255);
	}
	else
	{
		widget->disable();
		widget->setTransparency(128);
	}
}

void MenuPage::setEnabledComboBox(tgui::ComboBox::Ptr widget, bool enable)
{
	setEnabled(widget, enable);
	sf::Uint8 a = enable ? 255 : 128;
	sf::Color bg = widget->getBackgroundColor();
	sf::Color border = widget->getBorderColor();
	bg.a = a;
	border.a = a;
	widget->setBackgroundColor(bg);
	widget->setBorderColor(border);
}

void MenuPage::setChecked(tgui::Checkbox::Ptr widget, const bool checked)
{
	if (checked)
		widget->check();
	else
		widget->uncheck();
}