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
	JumpSystem(LayerManager* layerManager);
	~JumpSystem();
	void configure(entityx::EventManager &event_manager) override;
	void receive(const SkillEvent& event);
	bool targetIsOutOfBounds(int toX, int toY);
	void update(EntityManager &entityManager, EventManager &eventManager, TimeDelta dt) override;
	
	static void adjustXY_RelatingToTheDirection(int* x, int* y, int step, Direction direction);
private:
	LayerManager* m_layerManager;
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
	void adjustCellsIfOutOfBounds(int* fromX, int* toX, int* fromY, int* toY);
	void adjustHeightForBlockedTiles(const bool wasBlocked, const bool targetIsBlocked, float* beginHeight, float* endHeight, const float offHeight);
	float getDeltaOf(float coord2, float coord1);
	float getXCenterCoords(int cellX);
	float getYCenterCoords(int cellY);
	float getXCoords(int cellX);
	float getYCoords(int cellY);
};
