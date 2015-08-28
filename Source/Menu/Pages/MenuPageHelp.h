#pragma once
#include "../MenuPage.h"

class MenuPageHelp : public MenuPage
{
public:
	MenuPageHelp(Menu &menu);
	tgui::Panel::Ptr createPanel(const string& title);
	void createItem(tgui::Panel::Ptr panel, const string &icon, const string &name, const string &description);

private:
	void onPrev();
	void onNext();

private:
	std::vector<tgui::Panel::Ptr> m_panels;
	int m_panelIndex = 0;
	float m_yPos;
};
