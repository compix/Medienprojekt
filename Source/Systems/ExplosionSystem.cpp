#include "ExplosionSystem.h"
#include "../Components/ExplosionComponent.h"
#include "../Components/CellComponent.h"
#include "../Components/LayerComponent.h"
#include "../Components/LinkComponent.h"
#include "../Components/DestructionComponent.h"
#include "../GameGlobals.h"
#include "../Components/SolidBlockComponent.h"
#include "../Components/EffectComponent.h"
#include "../Components/InventoryComponent.h"
#include "../Components/BombComponent.h"
#include "../Components/FloorComponent.h"
#include "../Components/HealthComponent.h"
#include "../Components/PortalComponent.h"

ExplosionSystem::ExplosionSystem(LayerManager* layerManager)
	:m_layerManager(layerManager) {}

void ExplosionSystem::update(entityx::EntityManager& entities, entityx::EventManager& events, entityx::TimeDelta dt)
{
	std::vector<ExplosionSpreadRequest> explosionSpreadRequests;

	for (auto entity : entities.entities_with_components<ExplosionComponent, CellComponent, LayerComponent>())
	{
		auto layer = entity.component<LayerComponent>();
		auto spread = entity.component<SpreadComponent>();
		auto cell = entity.component<CellComponent>();

		if (!spread || !cell || spread->stopped)
			continue;

		spread->timeTillNext -= (float) dt;

		if (spread->timeTillNext <= 0.f)
		{
			if (spread->range > 0)
			{
				int nextCellX = spread->direction == Direction::LEFT ? cell->x - 1 : spread->direction == Direction::RIGHT ? cell->x + 1 : cell->x;
				int nextCellY = spread->direction == Direction::UP ? cell->y - 1 : spread->direction == Direction::DOWN ? cell->y + 1 : cell->y;
				
				int nextRange = spread->range - 1;
				for (auto& e : m_layerManager->getEntities(layer->layer, nextCellX, nextCellY))
				{
					if (e.has_component<ExplosionComponent>() || e.has_component<EffectComponent>() || e.has_component<InventoryComponent>() || e.has_component<PortalComponent>())
						continue;

					nextRange = 0;
					break;
				}
				if (!m_layerManager->hasEntityWithComponent<SolidBlockComponent>(layer->layer, nextCellX, nextCellY))
				{
					// Don't create the explosion right away to avoid multiple spreading in one direction in one frame. This fixes a low fps bug with portals.
					explosionSpreadRequests.push_back(ExplosionSpreadRequest(nextCellX, nextCellY, spread->direction, nextRange, spread->spreadTime));				
				}
			}
			spread->stopped = true;
		}
	}

	// Handle explosion spread requests
	for (auto& r : explosionSpreadRequests)
		GameGlobals::entityFactory->createExplosion(r.x, r.y, r.direction, r.range, r.spreadTime);
}