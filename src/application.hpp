#pragma once

#include <memory>

#include "camera.hpp"
#include "renderer.hpp"
#include "timer.hpp"
#include "window.hpp"


class Application
{
public:
	Application();
	~Application();

	int Run();

	Window& GetWindow() const;
	Renderer& GetRenderer() const;
	Camera& GetCamera() const;

	LRESULT WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	static Application* const GetApplication();

private:
	void UpdateFrameStats(float dt);

private:
	std::unique_ptr<Window> m_pWindow;
	std::unique_ptr<Renderer> m_pRenderer;
	std::unique_ptr<Camera> m_pCamera;
	std::unique_ptr<Timer> m_pTimer;

	using FrameStatsCollector = std::pair<float, int>;  // <time delta, frames count>
	FrameStatsCollector m_frameStatsCollector;

	static HWND s_hWnd;
};
