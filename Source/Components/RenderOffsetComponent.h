#pragma once

struct RenderOffsetComponent
{
	RenderOffsetComponent(float xOffset, float yOffset) : xOffset(xOffset), yOffset(yOffset){}

	float xOffset, yOffset;
	bool scheduledForRemoval = false;	//entfernt Componente erst beim Rendern
};