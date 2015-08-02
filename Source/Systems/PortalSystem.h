#pragma once
#include "../Components/BodyComponent.h"
#include "../Components/PortalComponent.h"


#include "../LayerManager.h"
#include "../Components/CellComponent.h"


using namespace ECS;

class PortalSystem : public EntitySystem<PortalSystem>
{
public:
	PortalSystem(LayerManager* layerManager);
	~PortalSystem();
	void addedToEngine(Engine *engine) override;
	b2Vec2 fitEntityIntoCell(CellComponent* cellComponent);
	void update(float dt) override;

private:
	void onEntityDestroyed(Entity *entity);
	void onCreatePortal(Entity *triggerEntity);
	void onTimeout(Entity *affectedEntity);
	
private:
	LayerManager* m_layerManager;
	std::multimap<uint64_t, Entity> m_portals; //erlaubt mehrere Values zu einem Key 
};