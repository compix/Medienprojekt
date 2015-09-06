#pragma once
#include <entityx/System.h>
#include "../Events/Phase2StartedEvent.h"
#include <SFML/Graphics/Text.hpp>

class NotificationSystem : public entityx::System<NotificationSystem>, public entityx::Receiver<NotificationSystem>
{
public:
	NotificationSystem(uint8_t levelWidth, uint8_t levelHeight);

	void update(entityx::EntityManager& entities, entityx::EventManager& events, entityx::TimeDelta dt) override;

	void configure(entityx::EventManager& events) override;
	void receive(const Phase2StartedEvent& event);

private:
	void showMessage(const std::string& message);

private:
	uint8_t m_levelWidth;
	uint8_t m_levelHeight;

	sf::Font m_font;
	sf::Text m_text;

	std::string m_message;
    const float m_messageTime;
	float m_messageTimer;
	bool m_showingMessage;
};
