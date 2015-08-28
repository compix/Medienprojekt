#pragma once
#include "../MenuPage.h"

class MenuPageCredits : public MenuPage
{
public:
	MenuPageCredits(Menu &menu);

private:
	void createSection(float x, float &y, const std::string &name);
	void createSubsection(float x, float &y, const std::string &name);
};
