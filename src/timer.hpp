#pragma once

#include <chrono>


class Timer
{
	using HRclock = std::chrono::high_resolution_clock;

public:
	Timer();

	float GetDelta();

private:
	HRclock::time_point m_lastTP;
};
