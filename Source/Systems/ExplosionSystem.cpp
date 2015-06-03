#include "ExplosionSystem.h"
#include "../Components/ExplosionComponent.h"
#include "../Components/CellComponent.h"
#include "../Components/LayerComponent.h"
#include "../Components/LinkComponent.h"
#include "../Components/DestructionComponent.h"
#include "../GameGlobals.h"
#include "../Components/SolidBlockComponent.h"
#include "../Components/SmokeComponent.h"
#include "../Components/InventoryComponent.h"

ExplosionSystem::ExplosionSystem(LayerManager* layerManager)
	:m_layerManager(layerManager) {}

void ExplosionSystem::update(entityx::EntityManager& entities, entityx::EventManager& events, entityx::TimeDelta dt)
{
	for (auto entity : entities.entities_with_components<ExplosionComponent, CellComponent, LayerComponent, LinkComponent>())
	{
		auto layer = entity.component<LayerComponent>();
		auto link = entity.component<LinkComponent>();

		vector<Entity> scheduled;

		bool allStopped = true;

		for (auto fire : link->links)
		{
			auto spread = fire.component<SpreadComponent>();
			auto cell = fire.component<CellComponent>();

			if (!spread->stopped)
				allStopped = false;
			else
				continue;

			if (!spread || !cell)
				continue;			

			spread->timeTillNext -= (float) dt;

			if (spread->timeTillNext <= 0.f)
			{
				if (spread->range > 0)
				{
					int nextRow = spread->direction == Direction::UP ? cell->y - 1 : spread->direction == Direction::DOWN ? cell->y + 1 : cell->y;
					int nextCol = spread->direction == Direction::LEFT ? cell->x - 1 : spread->direction == Direction::RIGHT ? cell->x + 1 : cell->x;
					int nextRange = spread->range - 1;

					for (auto& e : m_layerManager->getEntities(layer->layer, nextCol, nextRow))
					{
						if (e.has_component<ExplosionComponent>() || e.has_component<SmokeComponent>() || e.has_component<InventoryComponent>())
							continue;

						nextRange = 0;
						break;
					}

					if (!m_layerManager->has<SolidBlockComponent>(layer->layer, nextCol, nextRow))
					{
						scheduled.push_back(GameGlobals::entityFactory->createExplosion(nextRow, nextCol, spread->direction, nextRange, spread->spreadTime));
					}				
				}

				spread->stopped = true;
			}
		}


		if (allStopped)
		{
			if (!entity.has_component<DestructionComponent>())
				entity.assign<DestructionComponent>(0.5f);
		}
		else
			for (auto e : scheduled)
				link->links.push_back(e);
	}
}