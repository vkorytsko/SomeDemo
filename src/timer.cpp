#include "timer.hpp"


Timer::Timer()
{
	m_lastTP = HRclock::now();
}


float Timer::GetDelta()
{
	const HRclock::time_point currentTP = HRclock::now();
	const std::chrono::duration<float> dt = currentTP - m_lastTP;
	m_lastTP = currentTP;

	return dt.count();
}
