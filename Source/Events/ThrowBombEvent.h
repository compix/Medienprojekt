#pragma once
#include <entityx/Entity.h>

struct ThrowBombEvent
{
	ThrowBombEvent(entityx::Entity whoThrows)
		: whoThrows(whoThrows) {}

	entityx::Entity whoThrows;

};
