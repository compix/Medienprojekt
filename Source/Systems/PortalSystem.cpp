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


using namespace entityx;

PortalSystem::PortalSystem(LayerManager* layerManager)
{
	m_layerManager = layerManager;
}

PortalSystem::~PortalSystem()
{
	m_portals.clear();
	GameGlobals::events->unsubscribe<TimeoutEvent>(*this);
	GameGlobals::events->unsubscribe<ComponentAddedEvent<PortalComponent>>(*this);
	GameGlobals::events->unsubscribe<CreatePortalEvent>(*this);
	GameGlobals::events->unsubscribe<EntityDestroyedEvent>(*this);
}

void PortalSystem::configure(entityx::EventManager& events)
{
	events.subscribe<TimeoutEvent>(*this);
	events.subscribe<ComponentAddedEvent<PortalComponent>>(*this);
	events.subscribe<CreatePortalEvent>(*this);
	events.subscribe<EntityDestroyedEvent>(*this);
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

		if (!entityCollection.empty())
		{
			auto owner = portal.component<OwnerComponent>()->entity;

			std::pair<std::multimap<Entity::Id, Entity>::iterator, std::multimap<Entity::Id, Entity>::iterator> ppp;
			ppp = m_portals.equal_range(owner.id());
			for (std::multimap<Entity::Id, Entity>::iterator it2 = ppp.first; it2 != ppp.second; ++it2)
			{
				if ((*it2).second.id() != portal.id()) //Wenn das gegenstück zum Portal gefunden ist.
				{
					for (Entity e : entityCollection)
					{
						if (!e.has_component<PortalMarkerComponent>() && e.id() != portal.id()){ //es kann Teleportiert werden, wenn es nicht markiert oder das Portal selber ist.
							if (e.has_component<BodyComponent>())
							{
								e.component <BodyComponent>()->body->SetTransform(fitEntityIntoCell((*it2).second.component<CellComponent>().get()), 0);
							}
							else
							{
								uint8_t cellX, cellY;
								cellX = (*it2).second.component<CellComponent>()->x;
								cellY = (*it2).second.component<CellComponent>()->y;
								e.component<TransformComponent>()->x = GameConstants::CELL_WIDTH * cellX + GameConstants::CELL_WIDTH*0.5f;
								e.component<TransformComponent>()->y = GameConstants::CELL_HEIGHT * cellY + GameConstants::CELL_HEIGHT*0.5f;
							}

							e.assign<PortalMarkerComponent>((*it2).second.id()); //Marker wird zum gegenstück erstellt, da die Entity sich schon dort befindet.
						}
					}
					break;
				}
			}
		}
	}
}

void PortalSystem::receive(const TimeoutEvent& timeoutEvent)
{
	auto entity = timeoutEvent.affectedEntity;
	if (entity.has_component<PortalComponent>())
		entity.destroy();
}

void PortalSystem::receive(const entityx::ComponentAddedEvent<PortalComponent>& event)
{
	auto entity = event.entity;
	assert(entity.has_component<OwnerComponent>());
}

void PortalSystem::receive(const entityx::EntityDestroyedEvent& event)
{
	auto entity = event.entity;

	if (!entity.valid())
		return;

	if (entity.has_component<PortalComponent>())
	{
		auto owner = entity.component<OwnerComponent>()->entity;

		std::pair<std::multimap<Entity::Id, Entity>::iterator, std::multimap<Entity::Id, Entity>::iterator> ppp;
		ppp = m_portals.equal_range(owner.id());
		for (std::multimap<Entity::Id, Entity>::iterator it2 = ppp.first; it2 != ppp.second; ++it2)
		{
			if ((*it2).second.id() == entity.id())
			{
				m_portals.erase(it2); //entfernt Portal, wenn dieses zerstört wird.
				break;				//da jedes Portal einmalig, kann auch schon aufgehört werden.
			}
		}
	}
	GameGlobals::entityFactory->destroyEntity(entity);
}

void PortalSystem::receive(const CreatePortalEvent& event)
{
	auto entity = event.triggerEntity;
	assert(entity.has_component<CellComponent>());

	uint8_t cellX, cellY;
	cellX = entity.component<CellComponent>()->x;
	cellY = entity.component<CellComponent>()->y;

	/**Auskomentierter Code, dient dazu das Portal vor den TriggerEntity zu setzen**/

	/*if (entity.has_component<DirectionComponent>()){ //Wenn es von einer Entity ausgeht, die eine Richtung hat, dann wird es vor der Entity platziert
		switch (entity.component<DirectionComponent>()->direction)
		{
		case Direction::UP: cellY--; break;
		case Direction::DOWN: cellY++; break;
		case Direction::LEFT: cellX--; break;
		case Direction::RIGHT: cellX++; break;
		default: break;
		}
	}*/


	
	//if (!m_layerManager->hasEntityWithComponent<BodyComponent>(GameConstants::MAIN_LAYER, cellX, cellY)){	//Wenn Portal gelegt werden kann
		if (m_portals.count(entity.id()) < 2){																//Maximal zwei Portale
			auto portal = GameGlobals::entityFactory->createPortal(cellX, cellY, entity);
			m_portals.insert(std::pair<Entity::Id, Entity>(entity.id(), portal));	//Verbindung zwischen den Portalen wird über die ID des Triggers hergestellt.
			entity.assign<PortalMarkerComponent>(portal.id());
			if (m_portals.count(entity.id()) == 2)
			{
				std::pair<std::multimap<Entity::Id, Entity>::iterator, std::multimap<Entity::Id, Entity>::iterator> ppp;
				ppp = m_portals.equal_range(entity.id());
				for (std::multimap<Entity::Id, Entity>::iterator it2 = ppp.first; it2 != ppp.second; ++it2)
				{
					if ((*it2).second.has_component<TimerComponent>())				//TIMER erneuern
					{
						(*it2).second.remove<TimerComponent>();
					}
					(*it2).second.assign<TimerComponent>(GameConstants::PORTAL_TIMER_LINKED);
				}
			}
		}
	//}
}

b2Vec2 PortalSystem::fitEntityIntoCell(CellComponent* cellComponent)
{
	return b2Vec2(PhysixSystem::toBox2D((cellComponent->x + 1)*((float)GameConstants::CELL_WIDTH) - (float)GameConstants::CELL_WIDTH / 2), PhysixSystem::toBox2D((cellComponent->y + 1)*((float)GameConstants::CELL_HEIGHT) - (float)GameConstants::CELL_HEIGHT / 2));
}


