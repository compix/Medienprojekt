#include <entityx/entityx.h>
#include "../Components/BodyComponent.h"
#include "../Events/TimeoutEvent.h"
#include "../LayerManager.h"
#include "../Components/CellComponent.h"
#include "../Components/JumpComponent.h"

struct SkillEvent;
struct TimeoutEvent;
using entityx::System;
using entityx::EntityManager;
using entityx::EventManager;
using entityx::TimeDelta;
using entityx::Receiver;
using entityx::ComponentAddedEvent;

class JumpSystem : public System<JumpSystem>, public Receiver<JumpSystem>
{
public:
	JumpSystem(LayerManager* layerManager, bool isClient);
	~JumpSystem();
	void configure(entityx::EventManager &event_manager) override;
	void receive(const SkillEvent& event);
	void update(EntityManager &entityManager, EventManager &eventManager, TimeDelta dt) override;
	
	static void adjustXY_RelatingToTheDirection(int* x, int* y, int step, Direction direction);
private:
	LayerManager* m_layerManager;
	bool m_isClient;

	void checkIfDegreeMustBeRecalculated(ComponentHandle<JumpComponent, EntityManager> jumpComp, bool targetBlocked);
	void calculateDegreeOfJumpComp(ComponentHandle<JumpComponent, EntityManager> jumpComp, float endHeight);
	void calculateXY_ForOffset(float* xPos, float* yPos, ComponentHandle<JumpComponent, EntityManager> jumpComp, float beginHeight);
	void setBodyAndTransformOfEntityToTarget(Entity jumpingEntity);
	void deactivateTimerForBombs(Entity jumpingEntity, ComponentHandle<JumpComponent, EntityManager> jumpComp);
	void activateTimerForBombs(Entity jumpingEntity);
	void deactivateCollisionForFlyingBodys(ComponentHandle<BodyComponent, EntityManager> body);
	void activateCollisionForFlyingBodys(ComponentHandle<BodyComponent, EntityManager> body);
	void removeRenderOffset(Entity jumping_entity);
	void jumpFunction(Entity jumpingEntity, ComponentHandle<JumpComponent, EntityManager> jumpComp, TimeDelta dt);
	void adjustXY_RelatingToTheDirection(int* x, int* y, int step, Direction direction, ComponentHandle<CellComponent> cellComponent, Entity* foundEntity);
	void adjustCellsIfOutOfBounds(int8_t* fromX, int8_t* toX, int8_t* fromY, int8_t* toY);
	void adjustHeightForBlockedTiles(const bool wasBlocked, const bool targetIsBlocked, float* beginHeight, float* endHeight, const float offHeight);
	float getDeltaOf(float coord2, float coord1);
	float getXCenterCoords(int8_t cellX);
	float getYCenterCoords(int8_t cellY);
	float getXCoords(int8_t cellX);
	float getYCoords(int8_t cellY);
};
