#include <entityx/Entity.h>

struct PunchEvent
{
	PunchEvent(entityx::Entity e, int punchDistance) : triggerEntity(e), punchDistance(punchDistance){}

	entityx::Entity triggerEntity;
	int punchDistance;
};