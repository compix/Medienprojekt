#include "ChatRenderSystem.h"
#include <iostream>
#include <format.h>
#include "../GameGlobals.h"
#include "../Events/ChatEvent.h"
#include "../Events/ExitEvent.h"
#include "../Utils/Math.h"
#include "../Events/MenuShowEvent.h"

ChatRenderSystem::ChatRenderSystem()
{
	if (!m_font.loadFromFile("Assets/fonts/DejaVuSans.ttf"))
	{
		std::cout << "Failed to load font Assets/fonts/DejaVuSans.ttf" << std::endl;
		GameGlobals::events->emit<ExitEvent>();
	}
	m_lineSpacing = m_font.getLineSpacing(18);

	for (int i = 0; i < MAX_CHAT_LINES; i++)
	{
		initText(m_entries[i].name);
		initText(m_entries[i].message);
	}
}

void ChatRenderSystem::initText(sf::Text& text)
{
	text.setFont(m_font);
	text.setCharacterSize(18);
	text.setStyle(sf::Text::Bold);
}

ChatRenderSystem::~ChatRenderSystem()
{
}

void ChatRenderSystem::configure(entityx::EventManager& events)
{
	events.subscribe<ChatEvent>(*this);
	events.subscribe<MenuShowEvent>(*this);
}

void ChatRenderSystem::update(EntityManager &entityManager, EventManager &eventManager, TimeDelta dt)
{
	if (m_moveUpTime > 0)
		m_moveUpTime -= dt;
	updateEntries(dt);
	if (m_visible)
	{
		GameGlobals::window->setView(*GameGlobals::screenView);
		renderEntries();
		GameGlobals::window->setView(*GameGlobals::gameView);
	}
}

void ChatRenderSystem::updateEntries(TimeDelta dt)
{
	int oldestEntry = m_oldestEntry;
	int index = m_oldestEntry;
	do {
		auto &entry = m_entries[index];
		if (entry.timeLeft <= 0)
			break;
		entry.timeLeft -= dt;
		if (index != m_oldestEntry && entry.timeLeft < 1.5)
			entry.timeLeft = 1.5;
		else if (entry.timeLeft <= 0)
		{
			m_moveUpTime = 0.5;
			oldestEntry = index + 1;
			if (oldestEntry == MAX_CHAT_LINES)
				oldestEntry = 0;
		}
		index++;
		if (index == MAX_CHAT_LINES)
			index = 0;
	} while (index != m_oldestEntry);
	m_oldestEntry = oldestEntry;
}

void ChatRenderSystem::renderEntries()
{
	sf::Color nameColor = sf::Color::Green;
	sf::Color messageColor = sf::Color::White;
	float x = 0;
	float y = 0;
	if (m_moveUpTime > 0)
		y = static_cast<float>(2 * m_moveUpTime * m_lineSpacing);
	int index = m_oldestEntry;
	do {
		auto &entry = m_entries[index];
		if (entry.timeLeft <= 0)
			return;

		nameColor.a = messageColor.a = static_cast<sf::Uint8>(255 * std::min(1.0, entry.timeLeft));

		entry.name.setPosition(x, y);
		entry.message.setPosition(x + entry.name.getLocalBounds().width, y);
		entry.name.setColor(nameColor);
		entry.message.setColor(messageColor);
		GameGlobals::window->draw(entry.name);
		GameGlobals::window->draw(entry.message);

		y += m_lineSpacing;

		index++;
		if (index == MAX_CHAT_LINES)
			index = 0;
	} while (index != m_oldestEntry);
}

void ChatRenderSystem::receive(const MenuShowEvent &evt)
{
	m_visible = !evt.visible;
}

void ChatRenderSystem::receive(const ChatEvent &evt)
{
	auto & entry = getChatEntry();
	entry.timeLeft = 5;
	entry.name.setString(fmt::format("{}: ", evt.name));
	entry.message.setString(evt.message);
}

ChatEntry& ChatRenderSystem::getChatEntry()
{
	int index = m_oldestEntry;
	do {
		if (m_entries[index].timeLeft <= 0)
		{
			std::cout << index << std::endl;
			return m_entries[index];
		}
		index++;
		if (index == MAX_CHAT_LINES)
			index = 0;
	} while (index != m_oldestEntry);

	std::cout << "free one" << std::endl;
	auto &entry = m_entries[m_oldestEntry++];
	if (m_oldestEntry == MAX_CHAT_LINES)
		m_oldestEntry = 0;
	return entry;
}