#include "ExplosionSystem.h"
#include "../Components/ExplosionComponent.h"
#include "../Components/CellComponent.h"
#include "../Components/LayerComponent.h"
#include "../GameGlobals.h"
#include "../Components/SolidBlockComponent.h"
#include "../Components/ExplosionStopComponent.h"
#include "../Components/InventoryComponent.h"

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

				if (m_layerManager->hasEntityWithComponent<SolidBlockComponent>(layer->layer, nextCellX, nextCellY))
					nextRange = 0;
				else
				{
					if (spread->bombType != BombType::GHOST && m_layerManager->hasEntityWithComponent<ExplosionStopComponent>(layer->layer, nextCellX, nextCellY))
						nextRange = 0;
				}

				if (!m_layerManager->hasEntityWithComponent<SolidBlockComponent>(layer->layer, nextCellX, nextCellY))
				{
					ExplosionSpreadRequest request(nextCellX, nextCellY, spread->direction, nextRange, spread->spreadTime, spread->bombType);
					explosionSpreadRequests.push_back(request);
				}
					
				else if (spread->bombType == BombType::LIGHTNING || spread->bombType == BombType::LIGHTNING_PEAK)
				{
					ExplosionSpreadRequest request(ExplosionSpreadRequest(cell->x, cell->y, spread->direction, 0, spread->spreadTime, spread->bombType));
					explosionSpreadRequests.push_back(request);
				}	
			}

			spread->stopped = true;
		}
	}

	// Explosion aren't created right away to avoid multiple spreading in one direction in one frame. This fixes a low fps bug with portals.
	// Handle explosion spread requests
	for (auto& r : explosionSpreadRequests)
		GameGlobals::entityFactory->createExplosion(r.x, r.y, r.direction, r.range, r.spreadTime, r.bombType);
}