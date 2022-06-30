#pragma once

#include <memory>

#include "renderer.hpp"
#include "timer.hpp"
#include "window.hpp"


class Application
{
public:
	Application();
	~Application();

	int Run();

private:
	void UpdateFrameStats(float dt);

private:
	std::unique_ptr<Window> m_pWindow;
	std::unique_ptr<Renderer> m_pRenderer;
	std::unique_ptr<Timer> m_pTimer;

	using FrameStatsCollector = std::pair<float, int>;  // <time delta, frames count>
	FrameStatsCollector m_frameStatsCollector;
};
