#include "MenuPageLoading.h"
#include "../../Events/PreloadEvent.h"
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

void MenuPageLoading::receive(const PreloadEvent& evt)
{
	m_loadingBar->setMaximum(evt.total);
	m_loadingBar->setValue(evt.progress);
	if (evt.progress == evt.total)
		GameGlobals::events->unsubscribe<PreloadEvent>(*this);
	m_section->setText(evt.nextSection);
	m_filename->setText(evt.nextFilename);
}