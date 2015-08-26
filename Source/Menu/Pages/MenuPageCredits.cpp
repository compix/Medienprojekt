#include "MenuPageCredits.h"

MenuPageCredits::MenuPageCredits(Menu &menu)
	:MenuPage(menu)
{
	createPicture(800, 600, "Assets/ui/background.png");
	createLabel(50, 50, "Credits");

	createLabel(50, 250, "Dennis Grass");
	createLabel(275, 350, "Wladimir Kröker");
	createLabel(500, 250, "Santo Pfingsten");
	createLabel(275, 450, "Natalie Carl");
}
