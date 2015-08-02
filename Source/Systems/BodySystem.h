#pragma once
#include "../Components/BodyComponent.h"

using namespace ECS;

class BodySystem : public EntitySystem<BodySystem>
{
public:
	void update(float dt) override;
};