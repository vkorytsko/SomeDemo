#include "application.hpp"

#include <iostream>
#include <stdint.h>

#include "exceptions.hpp"


const std::wstring NAME = L"Some Demo";
const uint16_t WIDTH = 1200;
const uint16_t HEIGHT = 800;
const float FRAME_STATS_UPDATE_PERIOD = 0.1f;


HWND Application::s_hWnd = nullptr;


Application::Application()
{
	std::clog << "Application initialization!" << std::endl;

	WIN_THROW_IF_FAILED(CoInitializeEx(nullptr, COINIT_MULTITHREADED));

	m_pWindow = std::make_unique<Window>(this, WIDTH, HEIGHT, NAME);
	s_hWnd = m_pWindow->GetHandle();

	m_pRenderer = std::make_unique<Renderer>();
	m_pCamera = std::make_unique<Camera>();
	m_pScene = std::make_unique<Scene>();
	m_pTimer = std::make_unique<Timer>();
}

Application::~Application()
{
	std::clog << "Application destroying!" << std::endl;

	CoUninitialize();
}

int Application::Run()
{
	while (true) {
		if (const auto exitcode = m_pWindow->ProcessMessages()) {
			return *exitcode;
		}

		const auto dt = m_pTimer->GetDelta();
		UpdateFrameStats(dt);

		if (!IsPaused())
		{
			m_pScene->Update(dt);
		}

		m_pCamera->Update(dt);

		m_pRenderer->BeginFrame();
		m_pScene->Draw();
		m_pRenderer->EndFrame();
	}
}

Window* Application::GetWindow() const
{
	if (!m_pWindow)
	{
		THROW_SOME_EXCEPTION(L"MISSING WINDOW!");
	}

	return m_pWindow.get();
}

Renderer* Application::GetRenderer() const
{
	if (!m_pRenderer)
	{
		THROW_SOME_EXCEPTION(L"MISSING RENDERER!");
	}

	return m_pRenderer.get();
}

Camera* Application::GetCamera() const
{
	if (!m_pCamera)
	{
		THROW_SOME_EXCEPTION(L"MISSING CAMERA!");
	}

	return m_pCamera.get();
}

bool Application::IsActive() const
{
	return m_isActive;
}

LRESULT Application::WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		case WM_KEYDOWN:
		case WM_SYSKEYDOWN:
		{
			const bool repeat = static_cast<bool>(lParam & 0x40000000);
			if (!repeat)
			{
				const auto keycode = static_cast<unsigned char>(wParam);
				if (keycode == VK_ESCAPE)
				{
					PostQuitMessage(0);
				}
				if (keycode == VK_CONTROL)
				{
					if (IsActive())
					{
						m_pWindow->ShowCursor(true);
					}
				}
			}
			break;
		}
		case WM_KEYUP:
		case WM_SYSKEYUP:
		{
			const auto keycode = static_cast<unsigned char>(wParam);
			if (keycode == VK_CONTROL)
			{
				if (IsActive())
				{
					m_pWindow->CenterCursor();
					m_pWindow->ShowCursor(false);
				}
			}
			if (keycode == VK_MENU)
			{
				return 0; // Why does it stuck?!?!
			}
			break;
		}
		case WM_ACTIVATE:
		{
			const auto active = static_cast<bool>(wParam & (WA_ACTIVE | WA_CLICKACTIVE));
			Activate(active);
			break;
		}
	}
	
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

Application* const Application::GetApplication()
{
	return reinterpret_cast<Application*>(GetWindowLongPtr(s_hWnd, GWLP_USERDATA));
}

void Application::Activate(bool active)
{
	m_isActive = active;

	if (m_pWindow)
	{
		m_pWindow->ShowCursor(!active);
		if (active)
		{
			m_pWindow->CenterCursor();
		}
	}
}

bool Application::IsPaused() const
{
	return IsActive() && (GetKeyState(VK_SPACE) < 0);
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
