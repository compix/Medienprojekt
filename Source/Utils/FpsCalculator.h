#pragma once
#include <cstdint>

class FpsCalculator
{
private:
	int index = 0;
	int64_t sum = 0;
    std::int64_t *fpsFrames;
    std::int64_t lastTime = -1;
    const int waitTime;
	const int cacheSize;
	int64_t nextUpdate;
    float fpsCount;

public:
    /**
     * @param waitTime time to wait before updating the time, to avoid flickering
     * @param cacheSize number of frames to cache
     * @param startValue the start value of all cache items (in milliseconds)
     */
	FpsCalculator(int waitTime, int cacheSize, int startValue);
	virtual ~FpsCalculator();

    float getFps() {
        return fpsCount;
    }

    void addFrame();

};

