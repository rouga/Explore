#pragma once

#include <iostream>
#include <chrono>

class Timer 
{
public:
	using Clock = std::chrono::high_resolution_clock;

	Timer() : mIsPaused(false), mElapsedPausedTime(0.0f) {}

	// Start or restart the timer
	void Start() 
	{
		mStartTime = Clock::now();
		mIsPaused = false;
		mElapsedPausedTime = 0.0f; // Reset paused time
	}

	// Stop the timer and return elapsed time in milliseconds
	float Stop() 
	{
		if (mIsPaused) 
		{
			// If paused, return the accumulated time
			return mElapsedPausedTime;
		}
		else 
		{
			auto endTime = Clock::now();
			std::chrono::duration<float, std::milli> duration = endTime - mStartTime;
			return duration.count() + mElapsedPausedTime;
		}
	}

	// Peek the elapsed time without stopping or resetting
	float Peek() const 
	{
		if (mIsPaused) 
		{
			return mElapsedPausedTime;
		}
		else 
		{
			auto now = Clock::now();
			std::chrono::duration<float, std::milli> duration = now - mStartTime;
			return duration.count() + mElapsedPausedTime;
		}
	}

	// Pause the timer
	void Pause() 
	{
		if (!mIsPaused) 
		{
			auto now = Clock::now();
			std::chrono::duration<float, std::milli> duration = now - mStartTime;
			mElapsedPausedTime += duration.count();
			mIsPaused = true;
		}
	}

	// Resume the timer
	void Resume() 
	{
		if (mIsPaused) 
		{
			mStartTime = Clock::now(); // Reset the start time to now
			mIsPaused = false;
		}
	}

	// Reset the timer
	void Reset() 
	{
		mIsPaused = false;
		mElapsedPausedTime = 0.0f;
		mStartTime = Clock::now();
	}

private:
	Clock::time_point mStartTime; // Start time point
	bool mIsPaused;                // Whether the timer is paused
	float mElapsedPausedTime;    // Total elapsed time while paused
};
