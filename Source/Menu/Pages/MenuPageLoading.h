#pragma once
#include "../MenuPage.h"




class MenuPageLoading : public MenuPage
{
public:
	MenuPageLoading(Menu &menu);

private:
	void onPreload(int progress, int total, string nextSection, const string &nextFilename);
private:
	tgui::LoadingBar::Ptr m_loadingBar;
	tgui::Label::Ptr m_section;
	tgui::Label::Ptr m_filename;
};
