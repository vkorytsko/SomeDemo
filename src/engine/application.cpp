#include "application.hpp"

#include <iostream>
#include <stdint.h>

#include <backends/imgui_impl_win32.h>

#include "exceptions.hpp"


// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);


namespace SD::ENGINE {

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

	m_pRenderer = std::make_unique<RENDER::Renderer>();
	m_pCamera = std::make_unique<Camera>();
	m_pSpace = std::make_unique<Space>();
	m_pTimer = std::make_unique<Timer>();


}

Application::~Application()
{
	std::clog << "Application destroying!" << std::endl;

	CoUninitialize();
}

int Application::Run()
{
	m_pSpace->Init();

	while (true) {
		if (const auto exitcode = m_pWindow->ProcessMessages())
		{
			m_pSpace->Destroy();

			return *exitcode;
		}

		const auto dt = m_pTimer->GetDelta();
		UpdateFrameStats(dt);

		// Simulate
		{
			m_pSpace->Simulate(dt);
		}

		// Update
		{
			m_pSpace->Update(dt);
			m_pCamera->Update(dt);
		}

		// Draw
		{
			m_pRenderer->Begin();

			// Space
			{
				m_pRenderer->BeginFrame();
				m_pSpace->DrawFrame();
				m_pRenderer->EndFrame();
			}
			
			// ImGui
			{
				m_pRenderer->BeginImGui();
				m_pSpace->DrawImGui();
				m_pRenderer->EndImGui();
			}

			m_pRenderer->End();
		}
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

RENDER::Renderer* Application::GetRenderer() const
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

LRESULT Application::WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
		return true;

	auto* io = ImGui::GetCurrentContext() ? &ImGui::GetIO() : nullptr;

	switch (uMsg)
	{
		case WM_KEYDOWN:
		case WM_SYSKEYDOWN:
		{
			if (io && io->WantCaptureKeyboard)
			{
				return true;
			}

			break;
		}
		case WM_KEYUP:
		case WM_SYSKEYUP:
		{
			if (io && io->WantCaptureKeyboard)
			{
				return true;
			}

			const auto keycode = static_cast<unsigned char>(wParam);
			if (keycode == 'C')
			{
				if (IsActive())
				{
					ActivateCamera(!m_isCameraActive);
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
		case WM_SIZE:
		{
			const uint16_t width = LOWORD(lParam);
			const uint16_t height = HIWORD(lParam);

			if (width > 0 && height > 0)
			{
				if (m_pWindow)
				{
					m_pWindow->Resize(width, height);
				}

				if (m_pRenderer)
				{
					m_pRenderer->OnWindowResize();
				}
			}

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

	ActivateCamera(m_isCameraActive);
}

void Application::ActivateCamera(bool active)
{
	m_isCameraActive = active;

	if (m_pWindow)
	{
		m_pWindow->ShowCursor(!m_isCameraActive);
		if (active)
		{
			m_pWindow->CenterCursor();
		}
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

}  // end namespace SD::ENGINE
