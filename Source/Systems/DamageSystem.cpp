#include "DamageSystem.h"
#include "../Components/DamageDealerComponent.h"
#include "../Components/CellComponent.h"
#include "../Components/LayerComponent.h"
#include "../Components/HealthComponent.h"
#include "../GameGlobals.h"


DamageSystem::DamageSystem(LayerManager* layerManager)
	:IteratingSystem(Family::all<DamageDealerComponent, CellComponent, LayerComponent>().get()), m_layerManager(layerManager)
{
}

void DamageSystem::processEntity(Entity *damageDealer, float deltaTime)
{
	auto cell = damageDealer->get<CellComponent>();
	auto ddComponent = damageDealer->get<DamageDealerComponent>();
	auto layer = damageDealer->get<LayerComponent>();

	// damage all other entities with a health component on the same cell
	for (auto entity : m_layerManager->getEntities(layer->layer, cell->x, cell->y))
	{
		if (entity->isValid())
		{
			auto health = entity->get<HealthComponent>();
			if (health)
			{
				GameGlobals::events->entityGotHit.emit(damageDealer, entity, ddComponent->damage);
			}
		}
	}
}