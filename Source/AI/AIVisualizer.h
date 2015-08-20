#pragma once
#include <SFML/Graphics.hpp>
#include "Actions/ActionType.h"

namespace entityx{
	class Entity;
}

class Graph;
struct AIPath;

enum class TextShapeType
{
	NONE,
	CIRCLE,
	RECT
};

class AIVisualizer
{
public:
	AIVisualizer();
	explicit AIVisualizer(Graph* graph);

	inline void setGraph(Graph* graph) { m_graph = graph; }

	void visualize(float deltaTime);

private:
	void drawPathInfoLegend();
	void visualizePathInfo();
	void visualizeNodeProperties();
	void visualizeDangerZones();
	void visualizeNodes();
	void drawText(const std::string& text, float x, float y, TextShapeType legendShapeType = TextShapeType::NONE, const sf::Color& color = sf::Color());

	void visualize(const AIPath& path);

	void visualizeAction(ActionType actionType, entityx::Entity& entity);

	void showMessage(const std::string& message);
private:
	Graph* m_graph;

	sf::Font m_font;
	sf::CircleShape m_circle;
	sf::RectangleShape m_rect;
	sf::Text m_text;

	bool m_visualizeNodes;
	bool m_visualizeProperties;
	bool m_visualizePathInfo;
	bool m_visualizeDangerZones;
	bool m_visualizeActions;
	bool m_keyPressed;

	float m_messageTimer;
	float const m_messageTime;
	bool m_showingMessage;
	std::string m_message;
	sf::Text m_textMessage;
};