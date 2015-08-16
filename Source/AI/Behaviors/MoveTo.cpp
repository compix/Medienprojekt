#include "MoveTo.h"
#include "../../GameConstants.h"
#include "../../Components/CellComponent.h"
#include "../../Components/InputComponent.h"
#include "../../Components/InventoryComponent.h"
#include "../../Components/AIComponent.h"

MoveTo::MoveTo(PathEngine* pathEngine, Entity& entity, LayerManager* layerManager, uint8_t targetX, uint8_t targetY)
	:FollowPath(AIPath(), layerManager), m_pathEngine(pathEngine), m_targetX(targetX), m_targetY(targetY)
{
	assert(entity.valid() && entity.has_component<CellComponent>());

	// Compute the path from the current position to the target
	auto targetNode = m_pathEngine->getGraph()->getNode(m_targetX, m_targetY);

	if (!targetNode->valid)
		return;

	auto cell = entity.component<CellComponent>();
	m_pathEngine->computePath(cell->x, cell->y, m_targetX, m_targetY, m_path);
}