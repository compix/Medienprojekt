#include "MenuPageLoading.h"

#include "../../Game.h"

MenuPageLoading::MenuPageLoading(Menu &menu)
	:MenuPage(menu)
{
	createPicture(800, 600, "Assets/ui/xubuntu_bg_aluminium.jpg");
	createLabel(50, 50, "Loading:");

	GameGlobals::events->subscribe<PreloadEvent>(*this);
	m_loadingBar = createLoadingBar(50, 275, 700, 50, 0, 100);
	m_section = createLabel(50, 100, "");
	m_filename = createLabel(50, 150, "");
}

void MenuPageLoading::onPreload(int progress, int total, string nextSection, const string &nextFilename)
{
	m_loadingBar->setMaximum(total);
	m_loadingBar->setValue(progress);
	if (progress == total)
		GameGlobals::events->unsubscribe<PreloadEvent>(*this);
	m_section->setText(nextSection);
	m_filename->setText(nextFilename);
}