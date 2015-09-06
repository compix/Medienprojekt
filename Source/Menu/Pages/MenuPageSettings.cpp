#include "MenuPageSettings.h"
#include "../../GameGlobals.h"
#include "../../Utils/InputManager.h"
#include <format.h>

MenuPageSettings::MenuPageSettings(Menu &menu)
	:MenuPage(menu)
{
	createPicture(800, 600, "Assets/ui/background.png");
	createLabel(30, 30, "Input Helper");
	auto label = createLabel(50, 80, "Press any key on the keyboard and it will show in the edit field.\n\
You can then select and copy the value to the clipboad with Ctrl + C.\n\
When you connect a Gamepad, the name will show in the edit field.\n\
The Gamepad Button can also be copied.\n\
The input configurations can be changed in the file input.json");
	label->setTextSize(16);

	float x1 = 30;
	float x2 = 200;
	float y = 200;
	label = createLabel(x1, y, "Last Button:");
	label->setTextSize(20);
	m_lastButton = createEditBox(x2, y - 7, 400, 30);

	y += 40;
	label = createLabel(x1, y, "Joystick:");
	label->setTextSize(20);
	m_joystickName = createEditBox(x2, y - 7, 400, 30);

	for (int j = 0; j<sf::Joystick::AxisCount; j++)
	{
		y += 40;
		m_axisNames[j] = createLabel(x1, y, GameGlobals::input->getAxisName((sf::Joystick::Axis)j));
		m_axisNames[j]->setTextSize(20);
		m_axisValues[j] = createLabel(x2, y, "");
		m_axisNames[j]->setTextSize(20);
	}

	for (unsigned int i = 0; i<sf::Joystick::Count; i++) {
		if (sf::Joystick::isConnected(i))
		{
			onJoystickConnected(i);
			break;
		}
	}

	GameGlobals::events->subscribe<sf::Event>(*this);
}

void MenuPageSettings::update()
{
	for (int j = 0; j<sf::Joystick::AxisCount; j++)
	{
		if (sf::Joystick::hasAxis(m_joystickId, (sf::Joystick::Axis)j))
		{
			m_axisValues[j]->setText(fmt::format("{:.1f}", sf::Joystick::getAxisPosition(m_joystickId, (sf::Joystick::Axis)j)));
		}
	}
}

void MenuPageSettings::onJoystickConnected(unsigned int id)
{
	m_joystickId = id;

	sf::Joystick::Identification info = sf::Joystick::getIdentification(m_joystickId);
	m_joystickName->setText(info.name.toAnsiString());

	for (int j = 0; j<sf::Joystick::AxisCount; j++)
	{
		bool hasAxis = sf::Joystick::hasAxis(m_joystickId, (sf::Joystick::Axis)j);
		m_axisValues[j]->setText("");
		setVisible(m_axisNames[j]->getLabel(), hasAxis);
		setVisible(m_axisNames[j]->getShadow(), hasAxis);
	}
}

void MenuPageSettings::receive(const sf::Event& evt)
{
	switch (evt.type)
	{
	case sf::Event::KeyPressed:
		if (!m_lastButton->isFocused())
			m_lastButton->setText(GameGlobals::input->getKeyName(evt.key.code));
		break;
	case sf::Event::JoystickButtonPressed:
		if (evt.joystickButton.joystickId == m_joystickId)
			m_lastButton->setText(fmt::format("{}", evt.joystickButton.button));
		break;
	case sf::Event::JoystickConnected:
		onJoystickConnected(evt.joystickConnect.joystickId);
		break;
	case sf::Event::JoystickDisconnected:
		if (evt.joystickConnect.joystickId == m_joystickId)
		{
			m_joystickId = -1;
			m_joystickName->setText("");
		}
		break;
    default: break;
	}
}
