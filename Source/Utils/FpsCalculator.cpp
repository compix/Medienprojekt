#include "FpsCalculator.h"
#include <chrono>
#include <algorithm>

FpsCalculator::FpsCalculator(int waitTime, int cacheSize, int startValue) : waitTime(waitTime), cacheSize(cacheSize)
{
	nextUpdate = 0;
	fpsFrames = new int64_t[cacheSize];
	for (int i = 0; i < cacheSize; i++)
	{
		fpsFrames[i] = startValue;
	}
	sum = cacheSize * startValue;
}

FpsCalculator::~FpsCalculator()
{
	delete[] fpsFrames;
}

void FpsCalculator::addFrame()
{
	using namespace std::chrono;
	milliseconds ms = duration_cast<milliseconds>(system_clock::now().time_since_epoch());

	int64_t time = ms.count();
	if (lastTime == -1)
	{
		lastTime = time;
	}

	int64_t delta = time - lastTime;
	lastTime = time;

	sum -= fpsFrames[index];
	sum += delta;
	fpsFrames[index] = delta;
	if (++index == cacheSize)
	{
		index = 0;
	}

	nextUpdate -= delta;
	if (nextUpdate <= 0)
	{
		nextUpdate = waitTime;
		fpsCount = 1000 / std::max(1.0f, (sum / (float) cacheSize));
	}
}