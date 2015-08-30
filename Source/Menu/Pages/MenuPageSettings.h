#pragma once
#include "../MenuPage.h"

class MenuPageSettings : public MenuPage, public entityx::Receiver<MenuPageSettings>
{
public:
	MenuPageSettings(Menu &menu);
	void update() override;
	void receive(const sf::Event& evt);

private:
	void onJoystickConnected(unsigned id);
	tgui::EditBox::Ptr m_lastButton;
	unsigned int m_joystickId = -1;
	tgui::EditBox::Ptr m_joystickName;
	std::shared_ptr<Label> m_axisNames[sf::Joystick::AxisCount];
	std::shared_ptr<Label> m_axisValues[sf::Joystick::AxisCount];
};
