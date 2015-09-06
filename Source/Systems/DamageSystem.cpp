#include "DamageSystem.h"
#include "../Components/DamageDealerComponent.h"
#include "../Components/CellComponent.h"
#include "../Components/LayerComponent.h"
#include "../Components/HealthComponent.h"
#include "../Events/EntityGotHitEvent.h"

DamageSystem::DamageSystem(LayerManager* layerManager)
	:m_layerManager(layerManager)
{
}

void DamageSystem::update(entityx::EntityManager& entities, entityx::EventManager& events, entityx::TimeDelta dt)
{
	for (auto damageDealer : entities.entities_with_components<DamageDealerComponent, CellComponent, LayerComponent>())
	{
		auto cell = damageDealer.component<CellComponent>();
		auto ddComponent = damageDealer.component<DamageDealerComponent>();

		// damage all other entities with a health component on that cell (main layer)
		for (auto entity : m_layerManager->getEntities(GameConstants::MAIN_LAYER, cell->x, cell->y))
		{
			if (entity.valid())
			{
				auto health = entity.component<HealthComponent>();
				if (health)
				{
					events.emit<EntityGotHitEvent>(damageDealer, entity, ddComponent->damage);
				}
			}
		}
	}
}
