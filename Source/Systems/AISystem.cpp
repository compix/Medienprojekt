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

void AISystem::update(entityx::EntityManager& entityManager, entityx::EventManager& eventManager, entityx::TimeDelta dt)
{
	entityx::Entity player;

	for (auto& p : entityManager.entities_with_components<InventoryComponent, CellComponent>())
	{
		if (!p.has_component<AIComponent>())
		{
			player = p;
			break;
		}
	}

	if (!player.valid())
		return;

	auto playerCell = player.component<CellComponent>();

	for (auto& e : entityManager.entities_with_components<AIComponent, CellComponent, InputComponent>())
	{
		auto inputComponent = e.component<InputComponent>();
		auto cell = e.component<CellComponent>();

		
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