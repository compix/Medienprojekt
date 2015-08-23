#include "AIVisualizer.h"
#include "../game.h"
#include "PathFinding/GraphNode.h"
#include "PathFinding/Graph.h"
#include "../Events/ExitEvent.h"
#include "PathFinding/AIPath.h"
#include "../Components/AIComponent.h"
#include "PathFinding/NodePathInfo.h"

AIVisualizer::AIVisualizer()
	:AIVisualizer(nullptr)
{
}

AIVisualizer::AIVisualizer(PathEngine* pathEngine)
	: m_pathEngine(pathEngine), m_visualizeNodes(false), m_visualizeProperties(false), m_visualizePathInfo(false), m_visualizeDangerZones(false), m_keyPressed(false), m_visualizeActions(false),
	m_messageTime(2.f), m_messageTimer(m_messageTime), m_showingMessage(false)
{
	if (!m_font.loadFromFile("Assets/fonts/DejaVuSans.ttf"))
	{
		std::cout << "Failed to load font Assets/fonts/DejaVuSans.ttf" << std::endl;
		GameGlobals::events->emit<ExitEvent>();
		return;
	}

	m_text.setFont(m_font);
	m_text.setCharacterSize(16);
	m_text.setColor(sf::Color::White);

	m_textMessage.setFont(m_font);
	m_textMessage.setCharacterSize(30);

	if (pathEngine)
		setPathEngine(pathEngine);
}

void AIVisualizer::setPathEngine(PathEngine* pathEngine)
{
	assert(pathEngine);
	m_pathEngine = pathEngine; 
	m_graph = m_pathEngine->getSimGraph();
}

void AIVisualizer::visualize(float deltaTime)
{
	if (!m_keyPressed)
	{
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::F1))
		{
			m_visualizeNodes = !m_visualizeNodes;
			showMessage(m_visualizeNodes ? "Visualizing nodes" : "Hiding node visualization");
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::F2))
		{
			m_visualizePathInfo = !m_visualizePathInfo;
			showMessage(m_visualizePathInfo ? "Visualizing path information" : "Hiding path information visualization");
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::F3))
		{
			m_visualizeDangerZones = !m_visualizeDangerZones;
			showMessage(m_visualizeDangerZones ? "Visualizing danger zones" : "Hiding danger zone visualization");
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::F4))
		{
			m_visualizeProperties = !m_visualizeProperties;
			showMessage(m_visualizeProperties ? "Visualizing node properties" : "Hiding node properties visualization");
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::F5))
		{
			m_visualizeActions = !m_visualizeActions;
			showMessage(m_visualizeActions ? "Visualizing AI actions" : "Hiding AI action visualization");
		}
	}

	m_keyPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::F1) || sf::Keyboard::isKeyPressed(sf::Keyboard::F2) ||
				   sf::Keyboard::isKeyPressed(sf::Keyboard::F3) || sf::Keyboard::isKeyPressed(sf::Keyboard::F4) || 
				   sf::Keyboard::isKeyPressed(sf::Keyboard::F5);

	if (m_visualizePathInfo)
	{
		visualizePathInfo(0);
	}
		

	if (m_visualizePathInfo)
		drawPathInfoLegend();

	if (!m_visualizePathInfo && m_visualizeNodes)
		visualizeNodes();

	if (m_visualizeProperties)
		visualizeNodeProperties();

	if (m_visualizeDangerZones)
		visualizeDangerZones();

	if (m_visualizeActions)
	{
		for (auto entity : GameGlobals::entities->entities_with_components<AIComponent>())
		{
			auto aiComponent = entity.component<AIComponent>();
			if (aiComponent && aiComponent->currentAction)
			{
				visualize(aiComponent->currentAction->path());
				visualizeAction(aiComponent->currentActionType, entity);
			}
		}
	}

	if (m_showingMessage)
	{
		m_messageTimer -= deltaTime;

		if (m_messageTimer < 0.f)
		{
			m_showingMessage = false;
			m_messageTimer = m_messageTime;
		}
		else
		{
			float t = m_messageTimer / m_messageTime;
			m_textMessage.setString(m_message);
			float textX = GameConstants::CELL_WIDTH * m_graph->m_width * 0.5f - m_textMessage.getLocalBounds().width * 0.5f;
			float textY = GameConstants::CELL_HEIGHT * m_graph->m_height * 0.5f - m_textMessage.getLocalBounds().height * 0.5f;
			m_textMessage.setPosition(textX, textY);
			m_textMessage.setColor(sf::Color(255, 255, 255, sf::Uint8(250 * t)));
			GameGlobals::window->draw(m_textMessage);
		}
	}
}

void AIVisualizer::showMessage(const std::string& message)
{
	m_showingMessage = true;
	m_messageTimer = m_messageTime;
	m_message = message;
}

void AIVisualizer::drawPathInfoLegend()
{
	int liNum = 0;
	drawText("UNVISITED", -100, ++liNum * 25.f, TextShapeType::CIRCLE, sf::Color(0, 255, 0));
	drawText("OPEN", -100, ++liNum * 25.f, TextShapeType::CIRCLE, sf::Color(255, 255, 0));
	drawText("CLOSED", -100, ++liNum * 25.f, TextShapeType::CIRCLE, sf::Color(255, 0, 255));
}

void AIVisualizer::visualizePathInfo(uint8_t task)
{
	m_circle.setRadius(20.f);
	m_circle.setOrigin(10, 10);
	m_rect.setFillColor(sf::Color(0, 255, 0, 100));
	m_rect.setSize(sf::Vector2f(2.f, float(GameConstants::CELL_HEIGHT)));

	for (auto x = 1; x < m_graph->m_width - 1; ++x)
	{
		for (auto y = 1; y < m_graph->m_height - 1; ++y)
		{
			auto& nodeInfo = m_pathEngine->m_nodeInfo[task][x][y];
			auto node = m_graph->m_nodeGrid[x][y];

			m_circle.setPosition(x * GameConstants::CELL_WIDTH + GameConstants::CELL_WIDTH*0.5f - 3.f, y * GameConstants::CELL_HEIGHT + GameConstants::CELL_HEIGHT*0.5f + 5.f);

			if (!node.valid)
				continue;

			switch (nodeInfo.state)
			{
			case NodeState::UNVISITED:
				m_circle.setFillColor(sf::Color(0, 255, 0, 50));
				break;
			case NodeState::OPEN:
				m_circle.setFillColor(sf::Color(255, 255, 0, 50));
				break;
			case NodeState::CLOSED:
				m_circle.setFillColor(sf::Color(50, 0, 50, 50));
				break;
			default: 
				break;
			}

			m_rect.setPosition(x * GameConstants::CELL_WIDTH + GameConstants::CELL_WIDTH*0.5f + 7.f, y * GameConstants::CELL_HEIGHT + GameConstants::CELL_HEIGHT*0.5f + 13.f);

			if (m_graph->hasNeighbor(&node, Direction::LEFT))
			{
				m_rect.setRotation(90);
				GameGlobals::window->draw(m_rect);
			}

			if (m_graph->hasNeighbor(&node, Direction::RIGHT))
			{
				m_rect.setRotation(-90);
				GameGlobals::window->draw(m_rect);
			}

			if (m_graph->hasNeighbor(&node, Direction::UP))
			{
				m_rect.setRotation(180);
				GameGlobals::window->draw(m_rect);
			}

			if (m_graph->hasNeighbor(&node, Direction::DOWN))
			{
				m_rect.setRotation(0);
				GameGlobals::window->draw(m_rect);
			}

			GameGlobals::window->draw(m_circle);
		}
	}
}

void AIVisualizer::visualizeNodeProperties()
{
	// Draw the legend
	float yoc = -5;
	m_rect.setRotation(0.f);
	m_rect.setSize(sf::Vector2f(5, 5));

	float liNum = 0;
	float xText = float(m_graph->m_width * GameConstants::CELL_WIDTH + GameConstants::CELL_WIDTH);
	drawText("Bomb", xText, ++liNum * 25, TextShapeType::RECT, sf::Color(255, 0, 0));
	drawText("Item", xText, ++liNum * 25, TextShapeType::RECT, sf::Color(0, 255, 0));
	drawText("Player", xText, ++liNum * 25, TextShapeType::RECT, sf::Color(0, 0, 255));
	drawText("Portal", xText, ++liNum * 25, TextShapeType::RECT, sf::Color(255, 255, 0));
	drawText("Block", xText, ++liNum * 25, TextShapeType::RECT, sf::Color(139, 69, 19));

	for (auto x = 1; x < m_graph->m_width - 1; ++x)
	{
		for (auto y = 1; y < m_graph->m_height - 1; ++y)
		{
			auto& node = m_graph->m_nodeGrid[x][y];

			int propertyNum = 0;
			m_rect.setSize(sf::Vector2f(5.f, float(GameConstants::CELL_HEIGHT)));
			if (node.properties.hasBomb)
			{
				m_rect.setFillColor(sf::Color(255, 0, 0));
				m_rect.setPosition(float(x * GameConstants::CELL_WIDTH + propertyNum * 5.f), float(y * GameConstants::CELL_HEIGHT));
				GameGlobals::window->draw(m_rect);
				propertyNum++;
			}

			if (node.properties.hasItem)
			{
				m_rect.setFillColor(sf::Color(0, 255, 0));
				m_rect.setPosition(float(x * GameConstants::CELL_WIDTH + propertyNum * 5.f), float(y * GameConstants::CELL_HEIGHT));
				GameGlobals::window->draw(m_rect);
				propertyNum++;
			}

			if (node.properties.hasPlayer)
			{
				m_rect.setFillColor(sf::Color(0, 0, 255));
				m_rect.setPosition(float(x * GameConstants::CELL_WIDTH + propertyNum * 5.f), float(y * GameConstants::CELL_HEIGHT));
				GameGlobals::window->draw(m_rect);
				propertyNum++;
			}

			if (node.properties.hasPortal)
			{
				m_rect.setFillColor(sf::Color(255, 255, 0));
				m_rect.setPosition(float(x * GameConstants::CELL_WIDTH + propertyNum * 5.f), float(y * GameConstants::CELL_HEIGHT));
				GameGlobals::window->draw(m_rect);
				propertyNum++;
			}

			if (node.properties.hasBlock)
			{
				m_rect.setFillColor(sf::Color(139, 69, 19));
				m_rect.setPosition(float(x * GameConstants::CELL_WIDTH + propertyNum * 5.f), float(y * GameConstants::CELL_HEIGHT));
				GameGlobals::window->draw(m_rect);
				propertyNum++;
			}
		}
	}
}

void AIVisualizer::visualizeDangerZones()
{
	m_circle.setRadius(20.f);
	m_circle.setOrigin(10, 10);

	for (auto x = 1; x < m_graph->m_width - 1; ++x)
	{
		for (auto y = 1; y < m_graph->m_height - 1; ++y)
		{
			auto& node = m_graph->m_nodeGrid[x][y];
			m_circle.setPosition(x * GameConstants::CELL_WIDTH + GameConstants::CELL_WIDTH*0.5f - 3.f, y * GameConstants::CELL_HEIGHT + GameConstants::CELL_HEIGHT*0.5f + 5.f);

			if (node.properties.affectedByExplosion)
			{
				m_circle.setFillColor(sf::Color(sf::Uint8(100 * node.properties.timeTillExplosion), 0, 0, 155));
				GameGlobals::window->draw(m_circle);
			}
		}
	}
}

void AIVisualizer::visualizeNodes()
{
	m_circle.setRadius(20.f);
	m_circle.setOrigin(10, 10);
	m_circle.setFillColor(sf::Color(0, 255, 0, 50));

	for (auto x = 1; x < m_graph->m_width - 1; ++x)
	{
		for (auto y = 1; y < m_graph->m_height - 1; ++y)
		{
			auto& node = m_graph->m_nodeGrid[x][y];

			m_circle.setPosition(x * GameConstants::CELL_WIDTH + GameConstants::CELL_WIDTH*0.5f - 3.f, y * GameConstants::CELL_HEIGHT + GameConstants::CELL_HEIGHT*0.5f + 5.f);

			if (node.valid)
			{
				m_circle.setFillColor(sf::Color(0, 255, 0, 40));
				GameGlobals::window->draw(m_circle);
			}
		}
	}
}

void AIVisualizer::drawText(const std::string& text, float x, float y, TextShapeType legendShapeType, const sf::Color& color)
{
	float yShapeOffset = 5;
	float xShapeOffset = 14;
	switch (legendShapeType)
	{
	case TextShapeType::CIRCLE:
		m_circle.setFillColor(color);
		m_circle.setRadius(8.f);
		m_circle.setOrigin(4, 4);
		m_circle.setPosition(x - xShapeOffset, y + yShapeOffset);
		GameGlobals::window->draw(m_circle);
		break;
	case TextShapeType::RECT:
		m_rect.setRotation(0.f);
		m_rect.setSize(sf::Vector2f(11, 11));
		m_rect.setFillColor(color);
		m_rect.setPosition(x - xShapeOffset, y + yShapeOffset);
		GameGlobals::window->draw(m_rect);
		break;
	case TextShapeType::NONE:
	default:
		break;
	}

	m_text.setString(text);
	m_text.setPosition(x, y);
	GameGlobals::window->draw(m_text);
}

void AIVisualizer::visualize(const AIPath& path)
{
	m_circle.setRadius(20);
	m_circle.setOrigin(10, 10);

	for (uint16_t i = 0; i < path.nodes.size(); ++i)
	{
		uint8_t x = path.nodes[i]->x;
		uint8_t y = path.nodes[i]->y;

		m_circle.setPosition(x * GameConstants::CELL_WIDTH + GameConstants::CELL_WIDTH*0.5f - 3.f, y * GameConstants::CELL_HEIGHT + GameConstants::CELL_WIDTH*0.5f + 5.f);
		m_circle.setFillColor(sf::Color(0, 0, 255, 50));

		GameGlobals::window->draw(m_circle);
	}
}

void AIVisualizer::visualizeAction(ActionType actionType, entityx::Entity& entity)
{
	auto transform = entity.component<TransformComponent>();
	m_text.setPosition(transform->x - GameConstants::CELL_WIDTH * 0.5f, transform->y - GameConstants::CELL_HEIGHT);

	switch (actionType)
	{
	case ActionType::DESTROY_BLOCK:
		m_text.setString("Destroying block.");
		break;
	case ActionType::WAIT:
		m_text.setString("Waiting for opportunities.");
		break;
	case ActionType::GET_ITEM:
		m_text.setString("Getting item.");
		break;
	case ActionType::GET_SAFE:
		m_text.setString("Getting safe.");
		break;
	case ActionType::KICK_BOMB:
		m_text.setString("Kicking bomb.");
		break;
	case ActionType::TRY_TO_SURVIVE:
		m_text.setString("Trying to survive.");
		break;
	case ActionType::PLACE_PORTAL:
		m_text.setString("Placing portal.");
		break;
	case ActionType::ATTACK_ENEMY:
		m_text.setString("Attacking enemy.");
		break;
	default:
		m_text.setString("No idea what I'm doing.");
		break;
	}

	GameGlobals::window->draw(m_text);
}