#pragma once
#include "../MenuPage.h"
#include <signal11/Signal.h>

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
	ConnectionScope m_connections;
};
