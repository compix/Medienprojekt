#pragma once
#include <rpcndr.h>

struct RenderOffsetComponent
{
	RenderOffsetComponent(float xOffset, float yOffset) : xOffset(xOffset), yOffset(yOffset){}

	float xOffset, yOffset;
	bool remove = false;	//entfernt Componente erst beim Rendern
};