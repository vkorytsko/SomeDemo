#pragma once

#include <memory>

#include "camera.hpp"
#include "renderer.hpp"
#include "scene.hpp"
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
	std::unique_ptr<Renderer> m_pRenderer;
	std::unique_ptr<Camera> m_pCamera;
	std::unique_ptr<Scene> m_pScene;
	std::unique_ptr<Timer> m_pTimer;

	using FrameStatsCollector = std::pair<float, int>;  // <time delta, frames count>
	FrameStatsCollector m_frameStatsCollector;

	static HWND s_hWnd;
};
