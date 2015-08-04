#pragma once
#include <ecstasy/core/Component.h>

struct LayerComponent: public Component<LayerComponent>
{
	int layer = 0;
	
	void reset() override {
		layer = 0;
	}
};