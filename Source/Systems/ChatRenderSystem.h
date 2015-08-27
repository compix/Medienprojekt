#pragma once
#include <entityx/entityx.h>
#include <SFML/Graphics.hpp>
#include "../EntityLayer.h"
#include "../LayerManager.h"

struct MenuShowEvent;
struct ChatEvent;
using entityx::System;
using entityx::EntityManager;
using entityx::EventManager;
using entityx::TimeDelta;
using entityx::Receiver;

struct ChatEntry
{
	sf::Text name;
	sf::Text message;
	TimeDelta timeLeft = 0;
};

const int MAX_CHAT_LINES = 8;

class ChatRenderSystem : public System<ChatRenderSystem>, public Receiver<ChatRenderSystem>
{
public:
	ChatRenderSystem();
	~ChatRenderSystem();

	void configure(entityx::EventManager& events) override;
	void update(EntityManager &entityManager, EventManager &eventManager, TimeDelta dt) override;
	void updateEntries(TimeDelta dt);
	void renderEntries();
	void receive(const MenuShowEvent& evt);
	void receive(const ChatEvent &evt);

private:
	void initText(sf::Text &text);
	ChatEntry &getChatEntry();

private:
	sf::Font m_font;
	ChatEntry m_entries[MAX_CHAT_LINES];
	int m_oldestEntry = 0;
	TimeDelta m_moveUpTime;
	float m_lineSpacing;
	bool m_visible = false;
};

