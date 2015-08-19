#include "FollowPath.h"
#include "../../GameConstants.h"
#include "../../Components/CellComponent.h"
#include "../../Components/InputComponent.h"
#include "../../Components/PortalComponent.h"

FollowPath::FollowPath()
	:m_layerManager(nullptr)
{
}

FollowPath::FollowPath(LayerManager* layerManager)
	:m_layerManager(layerManager)
{
}

FollowPath::FollowPath(const AIPath& path, LayerManager* layerManager)
	:m_path(path), m_layerManager(layerManager)
{
}

void FollowPath::operator()(entityx::Entity& entity)
{
	if (m_path.nodes.size() == 0)
		return;

	assert(entity);

	auto inputComponent = entity.component<InputComponent>();
	auto cell = entity.component<CellComponent>();
	auto transform = entity.component<TransformComponent>();

	assert(m_layerManager);
	assert(inputComponent && cell && transform);
	assert(m_path.nodes.size() > m_path.curNode);

	// Just an internal playerRadius value for collision avoidance
	float playerRadius = (GameConstants::CELL_WIDTH * 0.5f) * 0.7f;

	// Collision avoidance on debug doesn't work that well due to low fps -> huge position deltas 
#if defined(DEBUG) || defined(_DEBUG)
	playerRadius = (GameConstants::CELL_WIDTH * 0.5f) * 0.4f;;
#endif

	// Avoid collisions with blocks by checking if left, right, bot, top is too close to the AI
	int leftAvoidance  = (cell->x - static_cast<int>((transform->x - playerRadius) / GameConstants::CELL_WIDTH));
	int rightAvoidance = (cell->x - static_cast<int>((transform->x + playerRadius) / GameConstants::CELL_WIDTH));
	int botAvoidance   = (cell->y - static_cast<int>((transform->y - playerRadius) / GameConstants::CELL_HEIGHT));
	int topAvoidance   = (cell->y - static_cast<int>((transform->y + playerRadius) / GameConstants::CELL_HEIGHT));

	// Check if the entity reached a new cell on the path
	auto n1 = m_path.nodes[m_path.curNode];
	if (cell->x != n1->x || cell->y != n1->y)
		m_path.curNode = std::min(static_cast<unsigned int>(m_path.curNode + 1), m_path.nodes.size() - 1);

	if (m_path.curNode < m_path.nodes.size() - 1)
	{
		// Follow path
		auto n2 = m_path.nodes[m_path.curNode + 1];	

		int moveX = n2->x - n1->x;
		int moveY = n2->y - n1->y;

		// If there is a portal on the next cell then the above moveX, moveY calculation won't work
		// The entity has to enter the portal to be teleported to the next cell:
		auto portal = m_layerManager->getEntityWithComponent<PortalComponent>(GameConstants::MAIN_LAYER, n2->x, n2->y);
		if (portal)
		{
			if (portal.component<PortalComponent>()->otherPortal.valid())
			{
				auto otherPortal = portal.component<PortalComponent>()->otherPortal;
				auto portalCell = otherPortal.component<CellComponent>();
				// Move into the portal
				moveX = portalCell->x - n1->x;
				moveY = portalCell->y - n1->y;
			}
		}

		// It's impossible to cancel eachother out so either 1 or -1 will be set.
		int collisionAvoidanceX = leftAvoidance * abs(moveY) + rightAvoidance * abs(moveY);
		int collisionAvoidanceY = botAvoidance * abs(moveX) + topAvoidance * abs(moveX);

		// If there is going to be a collision on the path -> avoid it
		inputComponent->moveX = moveX + collisionAvoidanceX;
		inputComponent->moveY = moveY + collisionAvoidanceY;
	}

	// Try to move to the center of the goal-cell
	if (m_path.reachedGoal())
	{
		inputComponent->moveX = leftAvoidance + rightAvoidance;
		inputComponent->moveY = botAvoidance + topAvoidance;
	}
}