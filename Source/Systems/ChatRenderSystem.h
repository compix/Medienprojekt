#pragma once
#include <SFML/Graphics.hpp>
#include "../EntityLayer.h"
#include "../LayerManager.h"
#include <signal11/Signal.h>
#include <ecstasy/core/Engine.h>
#include <ecstasy/core/EntitySystem.h>

struct ChatEntry
{
	sf::Text name;
	sf::Text message;
	float timeLeft = 0;
};

const int MAX_CHAT_LINES = 8;
using std::string;

class ChatRenderSystem : public EntitySystem<ChatRenderSystem>
{
public:
	ChatRenderSystem();
	~ChatRenderSystem();

	void addedToEngine(Engine *engine) override;
	void update(float dt) override;
	void updateEntries(float dt);
	void renderEntries();
	
private:
	void onChat(const string &message, const string &name);

private:
	void initText(sf::Text &text);
	ChatEntry &getChatEntry();

private:
	sf::Font m_font;
	ChatEntry m_entries[MAX_CHAT_LINES];
	int m_oldestEntry = 0;
	float m_moveUpTime;
	float m_lineSpacing;
	ConnectionScope m_connections;
};

