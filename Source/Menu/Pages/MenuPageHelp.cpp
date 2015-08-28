#include "MenuPageHelp.h"

MenuPageHelp::MenuPageHelp(Menu &menu)
	:MenuPage(menu)
{
	createPicture(800, 600, "Assets/ui/background.png");

	auto panel = createPanel("Bombs:");
	createItem(panel, "Bomb", "Normal", "A simple bomb");
	createItem(panel, "GhostBomb", "Ghost-Bomb", "Explosion is not stopped by destructible blocks\n(explosion destroys in full range).");
	createItem(panel, "LightningBomb", "Lightning-Bomb", "1 Tile-Range explosion at the peak in all directions.");

	panel = createPanel("Passive Skills:");
	createItem(panel, "speed_multiplicator", "Speed Boost", "Boosts the movement speed.");
	createItem(panel, "BombRangeBoost", "Explosion-Range", "Boosts the explosion range.");
	createItem(panel, "bombCapBoost", "Bomb Capacity", "Allows the player to place 1 more bomb.");
	createItem(panel, "bomb_kick_skill", "Kick", "Allows the player to kick bombs in the facing\ndirection on contact: Bomb moves in that direction.");
	createItem(panel, "anti_magnet_skill", "Anti-Magnet", "If a kicked bomb hits the player the bomb is\nautomatically kicked back (the direction is reversed).");

	panel = createPanel("Active Skills:");
	createItem(panel, "portal_skill", "Portals", "Character can spawn 2 portals (1 at a time) at his\nposition.");
	createItem(panel, "punch_skill", "Punch", "Punch bombs 1 tile in front of the character.");
	createItem(panel, "holding_skill", "Bomb-Pickup", "Character can pick up 1 bomb if he is on top of the\nbomb (bomb pos = player pos).");
	createItem(panel, "blink_skill", "Blink", "Fast teleportation/blink in the facing direction\n(not instant).");


	tgui::Button::Ptr button = createButton(20, 110, 50, 25, "<");
	button->bindCallback(&MenuPageHelp::onPrev, this, tgui::Button::LeftMouseClicked);
	tgui::Button::Ptr button2 = createButton(80, 110, 50, 25, ">");
	button2->bindCallback(&MenuPageHelp::onNext, this, tgui::Button::LeftMouseClicked);

	m_panels[0]->show();
}

tgui::Panel::Ptr MenuPageHelp::createPanel(const string &title)
{
	tgui::Panel::Ptr panel = tgui::Panel::Ptr(*m_panel);
	panel->setPosition(20, 70);
	panel->setSize(580, 520);
	panel->setBackgroundColor(sf::Color::Transparent);

	std::shared_ptr<Label> label = std::make_shared<Label>(panel);
	label->setText("Help: " + title);
	label->setPosition(0, 0, 1);

	m_yPos = 80;

	panel->hide();
	m_panels.push_back(panel);
	return panel;
}

void MenuPageHelp::createItem(tgui::Panel::Ptr panel, const string& icon, const string& name, const string& description)
{
	tgui::Picture::Ptr picture(*panel);
	picture->load("Assets/textures/" + icon + ".png");
	picture->setSize(60, 60);
	picture->setPosition(0, m_yPos);
	m_yPos += 6;

	std::shared_ptr<Label> label = std::make_shared<Label>(panel);
	label->setText(name);
	label->setPosition(70, m_yPos, 1);
	label->setTextStyle(sf::Text::Bold);
	label->setTextSize(24);
	m_yPos += 30;

	std::shared_ptr<Label> descLabel = std::make_shared<Label>(panel);
	descLabel->setText(description);
	descLabel->setPosition(70, m_yPos, 1);
	descLabel->setTextSize(18);
	m_yPos += 50;
}

void MenuPageHelp::onPrev()
{
	m_panels[m_panelIndex]->hide();
	m_panelIndex--;
	if (m_panelIndex < 0)
		m_panelIndex = m_panels.size()-1;
	m_panels[m_panelIndex]->show();
}

void MenuPageHelp::onNext()
{
	m_panels[m_panelIndex]->hide();
	m_panelIndex++;
	if (m_panelIndex >= static_cast<int>(m_panels.size()))
		m_panelIndex = 0;
	m_panels[m_panelIndex]->show();
}