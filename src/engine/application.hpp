#pragma once

#include <memory>

#include "camera.hpp"
#include "render_system.hpp"
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
	RenderSystem* GetRenderSystem() const;
	Camera* GetCamera() const;

	bool IsActive() const { return m_isActive; };
	bool IsCameraActive() const { return m_isCameraActive; };

	LRESULT WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	static Application* const GetApplication();

private:
	void Activate(bool active);
	void ActivateCamera(bool active);
	void UpdateFrameStats(float dt);

private:
	bool m_isActive = false;
	bool m_isCameraActive = false;

	std::unique_ptr<Window> m_pWindow;
	std::unique_ptr<RenderSystem> m_pRenderSystem;
	std::unique_ptr<Camera> m_pCamera;
	std::unique_ptr<Space> m_pSpace;
	std::unique_ptr<Timer> m_pTimer;

	using FrameStatsCollector = std::pair<float, int>;  // <time delta, frames count>
	FrameStatsCollector m_frameStatsCollector;

	static HWND s_hWnd;
};

}  // end namespace SD::ENGINE
