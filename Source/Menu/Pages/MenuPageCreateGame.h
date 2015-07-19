#pragma once
#include "../MenuPage.h"
#include "../../GameConstants.h"
#include "MenuPageLobby.h"

class MenuPageCreateGame : public MenuPage
{
public:
	MenuPageCreateGame(Menu &menu);
	void onSubmit();
	void onChange();
	tgui::ComboBox::Ptr createSizeComboBox(float x, float y, float width, float height, int from, int to, int selected);

private:
	tgui::EditBox::Ptr m_name[GameConstants::MAX_PLAYERS];
	tgui::ComboBox::Ptr m_type[GameConstants::MAX_PLAYERS];
	tgui::ComboBox::Ptr m_playerCount;
	tgui::ComboBox::Ptr m_width;
	tgui::ComboBox::Ptr m_height;
	tgui::EditBox::Ptr m_port;
	tgui::Button::Ptr m_submitButton;
};
