#pragma once
#include <entityx/Entity.h>

struct PreloadEvent
{
	PreloadEvent(int progress, int total, string nextSection, string nextFilename) : progress(progress), total(total), nextSection(nextSection), nextFilename(nextFilename) {}

	int progress;
	int total;
	string nextSection;
	string nextFilename;
};