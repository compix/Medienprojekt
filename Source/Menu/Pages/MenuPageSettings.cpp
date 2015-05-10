#include "MenuPageSettings.h"

MenuPageSettings::MenuPageSettings(Menu &menu)
	:MenuPage(menu)
{
	createPicture(800, 600, "Assets/ui/xubuntu_bg_aluminium.jpg");
	createLabel(50, 50, "Settings");
}
