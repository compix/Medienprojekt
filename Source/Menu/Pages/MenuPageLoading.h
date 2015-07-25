#pragma once
#include "../MenuPage.h"

struct PreloadEvent;
using entityx::Receiver;

class MenuPageLoading : public MenuPage, public Receiver<Menu>
{
public:
	MenuPageLoading(Menu &menu);

	void receive(const PreloadEvent &evt);
private:
	tgui::LoadingBar::Ptr m_loadingBar;
	tgui::Label::Ptr m_section;
	tgui::Label::Ptr m_filename;
};
