#pragma once
#include "entityx/entityx.h"

using namespace entityx;

class Game : public EntityX
{
public:
	Game();
	~Game();

	void update(TimeDelta dt);
};

