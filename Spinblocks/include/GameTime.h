#pragma once

namespace GameTime
{
	double startTime;
	double lastFrameTime;
	double accumulator;
	double fixedDeltaTime;

	void Initialize(double startTimeValue)
	{
		startTime = startTimeValue;  // Just what time we're taking as starting the game, in case we want to use that for anything.
		lastFrameTime = startTime; // Because we haven't had a frame yet, initialize last frame to start time.
		accumulator = 0.0;
		fixedDeltaTime = 0.02;
	}
}