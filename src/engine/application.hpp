#pragma once

#include <memory>

#include "camera.hpp"
#include "renderer.hpp"
#include "space.hpp"
#include "timer.hpp"
#include "window.hpp"


namespace SD::ENGINE {

class Application
{
public:
	Application();
	~Application();

	int Run();

	Window* GetWindow() const;
	RENDER::Renderer* GetRenderer() const;
	Camera* GetCamera() const;

	bool IsActive() const;

	LRESULT WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	static Application* const GetApplication();

private:
	void Activate(bool active);
	bool IsPaused() const;
	void UpdateFrameStats(float dt);

private:
	bool m_isActive = false;

	std::unique_ptr<Window> m_pWindow;
	std::unique_ptr<RENDER::Renderer> m_pRenderer;
	std::unique_ptr<Camera> m_pCamera;
	std::unique_ptr<Space> m_pSpace;
	std::unique_ptr<Timer> m_pTimer;

	using FrameStatsCollector = std::pair<float, int>;  // <time delta, frames count>
	FrameStatsCollector m_frameStatsCollector;

	static HWND s_hWnd;
};

}  // end namespace SD::ENGINE
