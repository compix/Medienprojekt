#include "PortalSystem.h"

#include "../GameGlobals.h"
#include "../EntityFactory.h"
#include "../Components/OwnerComponent.h"

#include "../Components/DirectionComponent.h"
#include "../Components/CellComponent.h"
#include "../Components/LayerComponent.h"
#include "../Components/PortalMarkerComponent.h"
#include "../Components/TimerComponent.h"

PortalSystem::PortalSystem(LayerManager* layerManager)
{
	m_layerManager = layerManager;
}

PortalSystem::~PortalSystem()
{
	m_portals.clear();
	m_connections.removeAll();
}

void PortalSystem::addedToEngine(Engine *engine)
{
	m_portalMarkerEntities = engine->getEntitiesFor(Family::all<PortalMarkerComponent, CellComponent, LayerComponent>().get());
	m_portalEntities = engine->getEntitiesFor(Family::all<PortalComponent, CellComponent, LayerComponent>().get());
	m_connections += GameGlobals::events->timeout.connect(this, &PortalSystem::onTimeout);
	m_connections += GameGlobals::events->createPortal.connect(this, &PortalSystem::onCreatePortal);
	m_connections += engine->entityRemoved.connect(this, &PortalSystem::onEntityDestroyed);
}


void PortalSystem::update(float dt)
{
	// MARKER entfernen falls nötig
	for (Entity *markedEntity : *m_portalMarkerEntities) {
		auto mark = markedEntity->get<PortalMarkerComponent>();
		auto cellComponent = markedEntity->get<CellComponent>();
		auto layerComponent = markedEntity->get<LayerComponent>();

		auto portal = m_layerManager->getEntityWithComponent<PortalComponent>(layerComponent->layer, cellComponent->x, cellComponent->y);

		if (portal)
		{
			if (portal->getId() != mark->portalId)
			{
				markedEntity->remove<PortalMarkerComponent>();
			}
		}
		else
		{
			markedEntity->remove<PortalMarkerComponent>();
		}
	}

	//Teleportation
	for (Entity *portal : *m_portalEntities) {
		auto cellComponent = portal->get<CellComponent>();
		auto layerComponent = portal->get<LayerComponent>();

		EntityCollection entityCollection = m_layerManager->getEntities(layerComponent->layer, cellComponent->x, cellComponent->y);

		if (!entityCollection.empty())
		{
			std::pair<std::multimap<uint64_t, Entity*>::iterator, std::multimap<uint64_t, Entity*>::iterator> ppp;
			ppp = m_portals.equal_range(portal->get<OwnerComponent>()->entityId);
			for (std::multimap<uint64_t, Entity*>::iterator it2 = ppp.first; it2 != ppp.second; ++it2)
			{
				if ((*it2).second->getId() != portal->getId()) //Wenn das gegenstück zum Portal gefunden ist.
				{
					for (Entity *e : entityCollection)
					{
						if (!e->has<PortalMarkerComponent>() && e->getId() != portal->getId()){ //es kann Teleportiert werden, wenn es nicht markiert oder das Portal selber ist.
							if (e->has<BodyComponent>())
							{
								e->get <BodyComponent>()->body->SetTransform(fitEntityIntoCell((*it2).second->get<CellComponent>()), 0);
							}
							else
							{
								uint8_t cellX, cellY;
								cellX = (*it2).second->get<CellComponent>()->x;
								cellY = (*it2).second->get<CellComponent>()->y;
								e->get<TransformComponent>()->x = GameConstants::CELL_WIDTH * cellX + GameConstants::CELL_WIDTH*0.5f;
								e->get<TransformComponent>()->y = GameConstants::CELL_HEIGHT * cellY + GameConstants::CELL_HEIGHT*0.5f;
							}

							e->assign<PortalMarkerComponent>((*it2).second->getId()); //Marker wird zum gegenstück erstellt, da die Entity sich schon dort befindet.
						}
					}
					break;
				}
			}
		}
	}
}

void PortalSystem::onTimeout(Entity *affectedEntity)
{
	if (affectedEntity->has<PortalComponent>())
		getEngine()->removeEntity(affectedEntity);
}

void PortalSystem::onEntityDestroyed(Entity *entity)
{
	if (!entity->isValid())
		return;

	if (entity->has<PortalComponent>())
	{
		std::pair<std::multimap<uint64_t, Entity*>::iterator, std::multimap<uint64_t, Entity*>::iterator> ppp;
		ppp = m_portals.equal_range(entity->get<OwnerComponent>()->entityId);
		for (std::multimap<uint64_t, Entity*>::iterator it2 = ppp.first; it2 != ppp.second; ++it2)
		{
			if ((*it2).second->getId() == entity->getId())
			{
				m_portals.erase(it2); //entfernt Portal, wenn dieses zerstört wird.
				break;				//da jedes Portal einmalig, kann auch schon aufgehört werden.
			}
		}
	}
}

void PortalSystem::onCreatePortal(Entity *triggerEntity)
{
	assert(triggerEntity->has<CellComponent>());

	uint8_t cellX, cellY;
	cellX = triggerEntity->get<CellComponent>()->x;
	cellY = triggerEntity->get<CellComponent>()->y;

	/**Auskomentierter Code, dient dazu das Portal vor den TriggerEntity zu setzen**/

	/*if (triggerEntity->has<DirectionComponent>()){ //Wenn es von einer Entity ausgeht, die eine Richtung hat, dann wird es vor der Entity platziert
		switch (triggerEntity->get<DirectionComponent>()->direction)
		{
		case Direction::UP: cellY--; break;
		case Direction::DOWN: cellY++; break;
		case Direction::LEFT: cellX--; break;
		case Direction::RIGHT: cellX++; break;
		default: break;
		}
	}*/


	
	//if (!m_layerManager->hasEntityWithComponent<BodyComponent>(GameConstants::MAIN_LAYER, cellX, cellY)){	//Wenn Portal gelegt werden kann
		if (m_portals.count(triggerEntity->getId()) < 2){																//Maximal zwei Portale
			auto portal = GameGlobals::entityFactory->createPortal(cellX, cellY, triggerEntity);
			m_portals.insert(std::pair<uint64_t, Entity*>(triggerEntity->getId(), portal));	//Verbindung zwischen den Portalen wird über die ID des Triggers hergestellt.
			triggerEntity->assign<PortalMarkerComponent>(portal->getId());
			if (m_portals.count(triggerEntity->getId()) == 2)
			{
				std::pair<std::multimap<uint64_t, Entity*>::iterator, std::multimap<uint64_t, Entity*>::iterator> ppp;
				ppp = m_portals.equal_range(triggerEntity->getId());
				for (std::multimap<uint64_t, Entity*>::iterator it2 = ppp.first; it2 != ppp.second; ++it2)
				{
					if ((*it2).second->has<TimerComponent>())				//TIMER erneuern
					{
						(*it2).second->remove<TimerComponent>();
					}
					(*it2).second->assign<TimerComponent>(GameConstants::PORTAL_TIMER_LINKED);
				}
			}
		}
	//}
}

b2Vec2 PortalSystem::fitEntityIntoCell(CellComponent* cellComponent)
{
	return b2Vec2(PhysixSystem::toBox2D((cellComponent->x + 1)*((float)GameConstants::CELL_WIDTH) - (float)GameConstants::CELL_WIDTH / 2), PhysixSystem::toBox2D((cellComponent->y + 1)*((float)GameConstants::CELL_HEIGHT) - (float)GameConstants::CELL_HEIGHT / 2));
}


