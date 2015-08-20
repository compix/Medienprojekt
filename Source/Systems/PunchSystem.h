#include <entityx/entityx.h>
#include "../Components/BodyComponent.h"
#include "../Events/TimeoutEvent.h"
#include "../Events/PunchEvent.h"
#include "../LayerManager.h"
#include "../Components/CellComponent.h"
#include "../AI/Action.h"
#include "../Components/JumpComponent.h"

struct TimeoutEvent;
using entityx::System;
using entityx::EntityManager;
using entityx::EventManager;
using entityx::TimeDelta;
using entityx::Receiver;
using entityx::ComponentAddedEvent;

class PunchSystem : public System<PunchSystem>, public Receiver<PunchSystem>
{
public:
	PunchSystem(LayerManager* layerManager);
	~PunchSystem();
	void configure(entityx::EventManager &event_manager) override;
	void receive(const PunchEvent& event);
	void removeRenderOffset(Entity jumping_entity, ComponentHandle<JumpComponent> jump_comp, ComponentHandle<BodyComponent> body);
	void update(EntityManager &entityManager, EventManager &eventManager, TimeDelta dt) override;
	void jumpFunction(Entity jumpingEntity, ComponentHandle<JumpComponent, EntityManager> jumpComp, ComponentHandle<BodyComponent, EntityManager> body, TimeDelta dt);
	void deactivateTimerForBombs(Entity jumpingEntity, ComponentHandle<JumpComponent, EntityManager> jumpComp);
	void activateTimerForBombs(Entity jumpingEntity);
	void deactivateCollisionForFlyingBodys(ComponentHandle<BodyComponent, EntityManager> body);
	void activateCollisionForFlyingBodys(ComponentHandle<BodyComponent, EntityManager> body);
private:
	LayerManager* m_layerManager;
};
