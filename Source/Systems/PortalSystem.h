#pragma once
#include <entityx/entityx.h>
#include "../Components/BodyComponent.h"
#include "../Components/PortalComponent.h"
#include "../Events/TimeoutEvent.h"
#include "../LayerManager.h"
#include "../Components/CellComponent.h"

struct SkillEvent;
struct TimeoutEvent;
using entityx::System;
using entityx::EntityManager;
using entityx::EventManager;
using entityx::TimeDelta;
using entityx::Receiver;
using entityx::ComponentAddedEvent;

class PortalSystem : public System<PortalSystem>, public Receiver<PortalSystem>
{
public:
	PortalSystem(LayerManager* layerManager);
	~PortalSystem();
	void configure(entityx::EventManager &event_manager) override;
	void receive(const SkillEvent& event);
	void receive(const TimeoutEvent& timeoutEvent);
	void update(EntityManager &entityManager, EventManager &eventManager, TimeDelta dt) override;

private:
	void teleport(entityx::Entity& entity, uint8_t cellX, uint8_t cellY);

private:
	LayerManager* m_layerManager;
};