#include "PortalSystem.h"

#include "../GameGlobals.h"
#include "../EntityFactory.h"
#include "../Components/OwnerComponent.h"
#include "../Events/CreatePortalEvent.h"
#include "../Components/DirectionComponent.h"
#include "../Components/CellComponent.h"
#include "../Components/LayerComponent.h"
#include "../Components/PortalMarkerComponent.h"
#include "../Components/TimerComponent.h"
#include "../Components/InventoryComponent.h"
#include "../Components/BombComponent.h"


using namespace entityx;

PortalSystem::PortalSystem(LayerManager* layerManager)
{
	m_layerManager = layerManager;
}

PortalSystem::~PortalSystem()
{
	GameGlobals::events->unsubscribe<TimeoutEvent>(*this);
	GameGlobals::events->unsubscribe<CreatePortalEvent>(*this);
}

void PortalSystem::configure(entityx::EventManager& events)
{
	events.subscribe<TimeoutEvent>(*this);
	events.subscribe<CreatePortalEvent>(*this);
}


void PortalSystem::update(entityx::EntityManager& entityManager, entityx::EventManager& eventManager, entityx::TimeDelta dt)
{
	// MARKER entfernen falls nötig
	for (auto markedEntity : entityManager.entities_with_components<PortalMarkerComponent, CellComponent, LayerComponent>())
	{
		auto mark = markedEntity.component<PortalMarkerComponent>();
		auto cellComponent = markedEntity.component<CellComponent>();
		auto layerComponent = markedEntity.component<LayerComponent>();

		auto portal = m_layerManager->getEntityWithComponent<PortalComponent>(layerComponent->layer, cellComponent->x, cellComponent->y);

		if (portal)
		{
			if (portal.id() != mark->portalId)
			{
				markedEntity.remove<PortalMarkerComponent>();
			}
		}
		else
		{
			markedEntity.remove<PortalMarkerComponent>();
		}
	}

	//Teleportation
	for (auto portal : entityManager.entities_with_components<PortalComponent, CellComponent, LayerComponent>())
	{
		auto cellComponent = portal.component<CellComponent>();
		auto layerComponent = portal.component<LayerComponent>();

		EntityCollection entityCollection = m_layerManager->getEntities(layerComponent->layer, cellComponent->x, cellComponent->y);
		auto linkedPortal = portal.component<PortalComponent>()->otherPortal;

		if (!linkedPortal.valid())
			continue;

		for (Entity e : entityCollection)
		{
			// Es kann teleportiert werden, wenn es nicht markiert, kein Portal ist und auf der anderen Seite keine Bombe ist
			auto cell = linkedPortal.component<CellComponent>();
			if (!e.has_component<PortalMarkerComponent>() && !e.has_component<PortalComponent>())
			{
				// There is a bomb on the other portal -> teleportation not possible so add a marker
				if (m_layerManager->hasEntityWithComponent<BombComponent>(GameConstants::MAIN_LAYER, cell->x, cell->y))
				{
					e.assign<PortalMarkerComponent>(portal.id());
					continue;
				}
					
				teleport(e, cell->x, cell->y);
				e.assign<PortalMarkerComponent>(linkedPortal.id()); // Marker wird zum Gegenstück erstellt, da die Entity sich schon dort befindet.
			}
		}
	}
}

void PortalSystem::teleport(entityx::Entity& entity, uint8_t cellX, uint8_t cellY)
{
	assert(entity.has_component<TransformComponent>());

	auto transform = entity.component<TransformComponent>();
	transform->x = GameConstants::CELL_WIDTH * cellX  + GameConstants::CELL_WIDTH  * 0.5f;
	transform->y = GameConstants::CELL_HEIGHT * cellY + GameConstants::CELL_HEIGHT * 0.5f;

	if (entity.has_component<BodyComponent>())
	{
		auto body = entity.component<BodyComponent>()->body;
		body->SetTransform(PhysixSystem::toBox2D(transform->x, transform->y), 0);
	}

	m_layerManager->updateCell(entity);
}

void PortalSystem::receive(const TimeoutEvent& timeoutEvent)
{
	auto entity = timeoutEvent.affectedEntity;
	if (entity.has_component<PortalComponent>())
	{
		entity.destroy();
		GameGlobals::entityFactory->destroyEntity(entity);
	}
}

void PortalSystem::receive(const CreatePortalEvent& event)
{
	auto entity = event.triggerEntity;
	if (!entity.valid())
		return;
	assert(entity.has_component<CellComponent>() && entity.has_component<InventoryComponent>());

	auto cell = entity.component<CellComponent>();
	auto inventory = entity.component<InventoryComponent>();
	entityx::Entity firstPortal = inventory->placedPortals.first;
	auto newPortal = GameGlobals::entityFactory->createPortal(cell->x, cell->y, entity, firstPortal.valid());
	assert(newPortal.has_component<TimerComponent>());

	// Save portals in the inventory of the entity
	if (!inventory->placedPortals.first.valid())
	{
		inventory->placedPortals.first = newPortal;
	}	
	else
	{
		inventory->placedPortals.second = newPortal;
		firstPortal.remove<TimerComponent>();
		firstPortal.assign<TimerComponent>(GameConstants::PORTAL_TIMER_LINKED);

		// Link the portals:
		firstPortal.component<PortalComponent>()->otherPortal = newPortal;
		newPortal.component<PortalComponent>()->otherPortal = firstPortal;

		entity.assign<PortalMarkerComponent>(newPortal.id());
	}
}

