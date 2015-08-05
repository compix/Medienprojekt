#pragma once
#include <ecstasy/core/Component.h>

struct LayerComponent: public Component<LayerComponent>
{
	LayerComponent(int layer) : layer(layer) {}

	int layer;
};