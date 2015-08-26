#include "MenuPageSettings.h"

MenuPageSettings::MenuPageSettings(Menu &menu)
	:MenuPage(menu)
{
	createPicture(800, 600, "Assets/ui/background.png");
	createLabel(50, 50, "Settings");
}
