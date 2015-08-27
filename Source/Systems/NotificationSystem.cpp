#include "NotificationSystem.h"
#include "../Events/ExitEvent.h"
#include "../Menu/Menu.h"

NotificationSystem::NotificationSystem(uint8_t levelWidth, uint8_t levelHeight)
	: m_levelWidth(levelWidth), m_levelHeight(levelHeight), m_messageTime(3.5f), m_messageTimer(m_messageTime), m_showingMessage(false)
{
	if (!m_font.loadFromFile("Assets/fonts/DejaVuSans.ttf"))
	{
		std::cout << "Failed to load font Assets/fonts/DejaVuSans.ttf" << std::endl;
		GameGlobals::events->emit<ExitEvent>();
		return;
	}

	m_text.setFont(m_font);
	m_text.setCharacterSize(45);
}

void NotificationSystem::update(entityx::EntityManager& entities, entityx::EventManager& events, entityx::TimeDelta dt)
{
	if (m_showingMessage)
	{
		m_messageTimer -= float(dt);

		if (m_messageTimer < 0.f)
		{
			m_showingMessage = false;
			m_messageTimer = m_messageTime;
		}
		else
		{
			float t = m_messageTimer / m_messageTime;
			m_text.setString(m_message);
			float textX = GameConstants::CELL_WIDTH * m_levelWidth * 0.5f - m_text.getLocalBounds().width * 0.5f;
			float textY = GameConstants::CELL_HEIGHT * m_levelHeight * 0.5f - m_text.getLocalBounds().height * 0.5f;
			m_text.setPosition(textX, textY);
			m_text.setColor(sf::Color(255, 255, 255, sf::Uint8(250 * t)));
			GameGlobals::window->draw(m_text);
		}
	}
}

void NotificationSystem::configure(entityx::EventManager& events)
{
	events.subscribe<Phase2StartedEvent>(*this);
}

void NotificationSystem::receive(const Phase2StartedEvent& event)
{
	showMessage("Hurry up!");
}

void NotificationSystem::showMessage(const std::string& message)
{
	m_showingMessage = true;
	m_messageTimer = m_messageTime;
	m_message = message;
}