#include "AISystem.h"
#include "../Components/AIComponent.h"
#include "../Components/InputComponent.h"
#include "../Components/CellComponent.h"
#include "../Components/InventoryComponent.h"
#include "../Utils/PathFinding/PathEngine.h"

AISystem::AISystem(PathEngine* pathEngine)
	: m_pathEngine(pathEngine)
{
}

void AISystem::update(float dt)
{
	Entity *player = nullptr;

	m_pathEngine->update();
	//m_pathEngine->computePath(cell->x, cell->y, 11, 11, m_path);

	//m_pathEngine->visualize();
	//m_pathEngine->visualize(m_path);

	for (auto p : entityManager.entities_with_components<InventoryComponent, CellComponent>())
	{
		if (!p->has<AIComponent>())
		{
			player = p;
			break;
		}
	}

	if (!player)
		return;

	auto playerCell = player->get<CellComponent>();

	for (auto e : entityManager.entities_with_components<AIComponent, CellComponent, InputComponent>())
	{
		auto inputComponent = e->get<InputComponent>();
		auto cell = e->get<CellComponent>();

		
		auto node = m_pathEngine->getNode(playerCell->x, playerCell->y);
		auto playerNode = node;

		// Find path to player
		for (int i = -1; i < 4; ++i)
		{
			if (node->valid)
			{
				m_pathEngine->computePath(cell->x, cell->y, node->x, node->y, m_path);
				break;
			}
			
			node = m_pathEngine->getNeighbor(playerNode, static_cast<Direction>(i+1));
		}	

		if (m_path.nodeCount > 1)
		{
			// Follow path
			auto n1 = m_path.nodes[0];
			auto n2 = m_path.nodes[1];

			if (n1->cost >= NodeCost::DANGER_HIGH || n2->cost < NodeCost::DANGER_HIGH)
			{
				inputComponent->moveX = n2->x - n1->x;
				inputComponent->moveY = n2->y - n1->y;
			}
		}
	}
}