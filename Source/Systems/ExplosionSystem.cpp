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
	:IteratingSystem(Family::all<ExplosionComponent, CellComponent, LayerComponent>().get()), m_layerManager(layerManager) {}

void ExplosionSystem::processEntity(Entity *entity, float deltaTime)
{
	auto layer = entity->get<LayerComponent>();
	auto spread = entity->get<SpreadComponent>();
	auto cell = entity->get<CellComponent>();

	if (!spread || !cell || spread->stopped)
		return;

	spread->timeTillNext -= (float) deltaTime;

	if (spread->timeTillNext <= 0.f)
	{
		if (spread->range > 0)
		{
			int nextCellX = spread->direction == Direction::LEFT ? cell->x - 1 : spread->direction == Direction::RIGHT ? cell->x + 1 : cell->x;
			int nextCellY = spread->direction == Direction::UP ? cell->y - 1 : spread->direction == Direction::DOWN ? cell->y + 1 : cell->y;

			int nextRange = spread->range - 1;
			for (auto& e : m_layerManager->getEntities(layer->layer, nextCellX, nextCellY))
			{
				if (e->has<ExplosionComponent>() || e->has<EffectComponent>() || e->has<InventoryComponent>() || e->has<PortalComponent>())
					continue;

				nextRange = 0;
				break;
			}
			if (!m_layerManager->hasEntityWithComponent<SolidBlockComponent>(layer->layer, nextCellX, nextCellY))
			{
				GameGlobals::entityFactory->createExplosion(nextCellX, nextCellY, spread->direction, nextRange, spread->spreadTime);
			}				
		}
		spread->stopped = true;
	}
}