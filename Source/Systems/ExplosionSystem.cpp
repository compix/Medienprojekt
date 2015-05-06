#include "ExplosionSystem.h"
#include <Components/ExplosionComponent.h>
#include <Components/CellComponent.h>
#include <Components/LayerComponent.h>

ExplosionSystem::ExplosionSystem(EntityFactory* entityFactory, LayerManager* layerManager)
	:m_entityFactory(entityFactory), m_layerManager(layerManager) {}

void ExplosionSystem::update(entityx::EntityManager& entities, entityx::EventManager& events, entityx::TimeDelta dt)
{
	for (auto entity : entities.entities_with_components<ExplosionComponent, CellComponent, LayerComponent>())
	{
		auto explosion = entity.component<ExplosionComponent>();
		auto cell = entity.component<CellComponent>();
		auto layer = entity.component<LayerComponent>();

		explosion->timeTillNext -= dt;

		if (explosion->timeTillNext <= 0.f)
		{
			if (explosion->range > 0)
			{
				int nextRow = explosion->direction == ExplosionDirection::UP ? cell->y - 1 : explosion->direction == ExplosionDirection::DOWN ? cell->y + 1 : cell->y;
				int nextCol = explosion->direction == ExplosionDirection::LEFT ? cell->x - 1 : explosion->direction == ExplosionDirection::RIGHT ? cell->x + 1 : cell->x;
				int nextRange = explosion->range - 1;

				for (auto& e : m_layerManager->getEntities(layer->layer, nextCol, nextRow))
				{
					if(!e.has_component<ExplosionComponent>())
					{
						nextRange = 0;
						break;
					}
				}

				if (!m_layerManager->hasSolidBlock(layer->layer, nextCol, nextRow))
				{
					float lifeTime = explosion->range*explosion->spreadTime;
					m_entityFactory->createExplosion(nextRow, nextCol, explosion->direction, nextRange, explosion->spreadTime, lifeTime, true);
					explosion->range = 0; // Set the range of this explosion to 0 since it already spawned a new explosion entity.
				}
			}
		}
	}
}