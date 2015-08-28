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
	std::shared_ptr<Label> m_section;
	std::shared_ptr<Label> m_filename;
};
