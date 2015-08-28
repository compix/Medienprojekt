#include "MenuPageCredits.h"

MenuPageCredits::MenuPageCredits(Menu &menu)
	:MenuPage(menu)
{
	createPicture(800, 600, "Assets/ui/background.png");
	createLabel(50, 50, "Credits");

	float x = 50;
	float x2 = 300;

	float y = 200;
	createSection(x, y, "Developers: ");
	createSubsection(x, y, "Dennis Grass");
	createSubsection(x, y, "Wladimir Kröker");
	createSubsection(x, y, "Santo Pfingsten");

	y = 200;
	createSection(x2, y, "Graphics:");
	createSubsection(x2, y, "Natalie Carl");
	createSubsection(x2, y, "Dennis Grass");

	y = 400;
	createSection(x, y, "Sounds:");
	createSubsection(x, y, "Dennis Grass");

	y = 400;
	createSection(x2, y, "Music:");
	createSubsection(x2, y, "Jana Stürenburg");
}

void MenuPageCredits::createSection(float x, float &y, const std::string& name)
{
	y += 20;
	auto label = createLabel(x, y, name);
	label->setTextStyle(sf::Text::Bold);
	label->setTextSize(24);
	y += 40;
}

void MenuPageCredits::createSubsection(float x, float &y, const std::string& name)
{
	auto label = createLabel(x, y, name);
	label->setTextSize(20);
	y += 30;
}