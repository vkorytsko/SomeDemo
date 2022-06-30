#include "application.hpp"

#include <iostream>
#include <stdint.h>


const std::wstring NAME = L"Some Demo";
const uint16_t WIDTH = 1200;
const uint16_t HEIGHT = 800;
const float FRAME_STATS_UPDATE_PERIOD = 0.1f;


Application::Application()
{
	std::clog << "Application initialization!" << std::endl;

	m_pWindow = std::make_unique<Window>(WIDTH, HEIGHT, NAME);
	m_pRenderer = std::make_unique<Renderer>(m_pWindow->GetHandle());
	m_pTimer = std::make_unique<Timer>();
}

Application::~Application()
{
	std::clog << "Application destroying!" << std::endl;
}

int Application::Run()
{
	while (true) {
		if (const auto exitcode = m_pWindow->ProcessMessages()) {
			return *exitcode;
		}

		const auto dt = m_pTimer->GetDelta();
		UpdateFrameStats(dt);

		m_pRenderer->Update(dt);
		m_pRenderer->Render();
	}
}

void Application::UpdateFrameStats(float dt)
{
	auto& [timeDelta, framesCount] = m_frameStatsCollector;

	timeDelta += dt;
	framesCount++;

	if (timeDelta >= FRAME_STATS_UPDATE_PERIOD) {
		float fps = static_cast<float>(framesCount) / FRAME_STATS_UPDATE_PERIOD;
		float mspf = 1000.0f / fps;

		std::wstring frameStatsStr =
			NAME +
			L"    fps: " + std::to_wstring(fps) +
			L"   mspf: " + std::to_wstring(mspf);

		SetWindowText(m_pWindow->GetHandle(), frameStatsStr.c_str());

		timeDelta = 0.f;
		framesCount = 0;
	}
}
