#include "FollowPath.h"
#include "../GameConstants.h"
#include "../Utils/Random.h"
#include "../Components/CellComponent.h"
#include "../Components/InputComponent.h"
#include "../Components/PortalComponent.h"

FollowPath::FollowPath(Path path, LayerManager* layerManager)
	:m_path(path), m_lastMoveX(0), m_lastMoveY(0), m_layerManager(layerManager)
{
}

void FollowPath::update(entityx::Entity& entity)
{
	auto inputComponent = entity.component<InputComponent>();
	auto cell = entity.component<CellComponent>();
	auto transform = entity.component<TransformComponent>();

	assert(inputComponent && cell && transform);

	if (m_path.nodeCount > 1)
	{
		// Follow path
		auto n1 = m_path.nodes[0];
		auto n2 = m_path.nodes[1];

		// Avoid collisions with blocks by checking if left, right, bot, top is too close to the AI
		float playerRadius = (GameConstants::CELL_WIDTH * 0.5f) * 0.7f;

		int moveX = n2->x - n1->x;
		int moveY = n2->y - n1->y;

		auto portal = m_layerManager->getEntityWithComponent<PortalComponent>(GameConstants::MAIN_LAYER, n2->x, n2->y);
		if (portal)
		{
			if (portal.component<PortalComponent>()->otherPortal.valid())
			{
				auto otherPortal = portal.component<PortalComponent>()->otherPortal;
				auto protalCell = otherPortal.component<CellComponent>();
				moveX = protalCell->x - n1->x;
				moveY = protalCell->y - n1->y;
			}
		}

		int leftAvoidance  = (cell->x - static_cast<int>((transform->x - playerRadius) / GameConstants::CELL_WIDTH)) * abs(moveY);
		int rightAvoidance = (cell->x - static_cast<int>((transform->x + playerRadius) / GameConstants::CELL_WIDTH)) * abs(moveY);
		int collisionAvoidanceX = leftAvoidance + rightAvoidance;

		int botAvoidance = (cell->y - static_cast<int>((transform->y - playerRadius) / GameConstants::CELL_HEIGHT)) * abs(moveX);
		int topAvoidance = (cell->y - static_cast<int>((transform->y + playerRadius) / GameConstants::CELL_HEIGHT)) * abs(moveX);
		int collisionAvoidanceY = botAvoidance + topAvoidance;

		if ((collisionAvoidanceX-collisionAvoidanceY) == 0)
		{
			// There won't be any collisions so just move on the path
			inputComponent->moveX = moveX;
			inputComponent->moveY = moveY;
		}
		else
		{
			// There will be a collision on the path -> avoid it
			inputComponent->moveX = collisionAvoidanceX;
			inputComponent->moveY = collisionAvoidanceY;
		}
	}
}