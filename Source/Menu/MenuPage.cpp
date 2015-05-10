#include "MenuPage.h"
#include "Menu.h"

MenuPage::MenuPage(Menu &menu)
	:m_menu(menu), m_gui(menu.m_gui), m_events(menu.m_events), m_panel(m_gui)
{
	m_panel->setSize(800, 600);// fixme: replace by constants?
	m_panel->setBackgroundColor(sf::Color::Transparent);
	m_panel->hide();
}


MenuPage::~MenuPage() {}

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
	return chatBox;
}
