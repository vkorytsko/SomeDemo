#include "timer.hpp"


namespace SD::ENGINE {

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

float Timer::GetTotal()
{
	const HRclock::time_point currentTP = HRclock::now();
	const std::chrono::duration<float> dt = currentTP - m_startTP;

	return dt.count();
}

}  // end namespace SD::ENGINE
