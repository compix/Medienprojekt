#include <entityx/Entity.h>

struct PunchEvent
{
	PunchEvent(entityx::Entity e) : triggerEntity(e){}

	entityx::Entity triggerEntity;
};