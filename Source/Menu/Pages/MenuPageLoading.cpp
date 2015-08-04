#include "MenuPageLoading.h"

#include "../../Game.h"
#include "../../GameGlobals.h"

MenuPageLoading::MenuPageLoading(Menu &menu)
	:MenuPage(menu)
{
	createPicture(800, 600, "Assets/ui/xubuntu_bg_aluminium.jpg");
	createLabel(50, 50, "Loading:");

	m_connections += GameGlobals::events->preload.connect(this, &MenuPageLoading::onPreload);
	m_loadingBar = createLoadingBar(50, 275, 700, 50, 0, 100);
	m_section = createLabel(50, 100, "");
	m_filename = createLabel(50, 150, "");
}

void MenuPageLoading::onPreload(int progress, int total, string nextSection, const string &nextFilename)
{
	m_loadingBar->setMaximum(total);
	m_loadingBar->setValue(progress);
	if (progress == total)
		m_connections.removeAll();
	m_section->setText(nextSection);
	m_filename->setText(nextFilename);
}