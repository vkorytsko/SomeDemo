#pragma once

#include <chrono>


namespace SD::ENGINE {

class Timer
{
	using HRclock = std::chrono::high_resolution_clock;

public:
	Timer();

	float GetDelta();
	float GetTotal();

private:
	HRclock::time_point m_startTP;
	HRclock::time_point m_lastTP;
};

}  // end namespace SD::ENGINE
