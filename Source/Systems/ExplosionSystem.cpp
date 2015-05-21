#include "ExplosionSystem.h"
#include <Components/ExplosionComponent.h>
#include <Components/CellComponent.h>
#include <Components/LayerComponent.h>
#include <Components/LinkComponent.h>
#include <Components/DestructionComponent.h>

ExplosionSystem::ExplosionSystem(EntityFactory* entityFactory, LayerManager* layerManager)
	:m_entityFactory(entityFactory), m_layerManager(layerManager) {}

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

			spread->timeTillNext -= dt;

			if (spread->timeTillNext <= 0.f)
			{
				if (spread->range > 0)
				{
					int nextRow = spread->direction == Direction::UP ? cell->y - 1 : spread->direction == Direction::DOWN ? cell->y + 1 : cell->y;
					int nextCol = spread->direction == Direction::LEFT ? cell->x - 1 : spread->direction == Direction::RIGHT ? cell->x + 1 : cell->x;
					int nextRange = spread->range - 1;

					for (auto& e : m_layerManager->getEntities(layer->layer, nextCol, nextRow))
					{
						nextRange = 0;
						break;
					}

					if (!m_layerManager->hasSolidBlock(layer->layer, nextCol, nextRow))
					{
						scheduled.push_back(m_entityFactory->createExplosion(nextRow, nextCol, spread->direction, nextRange, spread->spreadTime, true));
					}				
				}

				spread->stopped = true;
			}
		}


		if (allStopped)
		{
			if (!entity.has_component<DestructionComponent>())
				entity.assign<DestructionComponent>(0.1f);
		}
		else
			for (auto e : scheduled)
				link->links.push_back(e);
	}
}