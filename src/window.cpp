#include "window.hpp"

#include <cassert>
#include <stdexcept>

#include "exceptions.hpp"


Window::WindowClass Window::WindowClass::m_wndClass;

Window::WindowClass::WindowClass()
    : m_hInstance(GetModuleHandle(nullptr))
{
    WNDCLASS windowClass = {};
    windowClass.lpfnWndProc = WindowSetupProc;
    windowClass.hInstance = GetInstance();
    windowClass.lpszClassName = GetName();

    WIN_THROW_IF_FAILED(RegisterClass(&windowClass))
}

Window::WindowClass::~WindowClass()
{
    UnregisterClass(GetName(), GetInstance());
}

const wchar_t* Window::WindowClass::GetName() noexcept
{
    return m_wndClass.m_wndClassName;
}

HINSTANCE  Window::WindowClass::GetInstance() noexcept
{
    return m_wndClass.m_hInstance;
}



Window::Window(int width, int height, const wchar_t* name)
    : m_width(width)
    , m_height(height)
{
    RECT windowRect = { 0, 0, m_width, m_height };
    AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, false);

    m_hWnd = CreateWindow(
        WindowClass::GetName(), name,
        WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
        windowRect.right - windowRect.left, windowRect.bottom - windowRect.top,
        nullptr, nullptr, WindowClass::GetInstance(), this);

    if (!m_hWnd) {
        WIN_THROW_LAST_EXCEPTION();
    }

    ShowWindow(m_hWnd, SW_SHOWDEFAULT);
}

Window::~Window()
{
    DestroyWindow(m_hWnd);
    m_hWnd = nullptr;
}

std::optional<int> Window::ProcessMessages()
{

    MSG msg;
    while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
        if (msg.message == WM_QUIT) {
            return static_cast<int>(msg.wParam);
        }

        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return {};
}

HWND Window::GetHandle() const
{
    return m_hWnd;
}

LRESULT CALLBACK Window::WindowSetupProc(_In_ HWND hWnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam)
{
    if (uMsg == WM_NCCREATE) {
        const CREATESTRUCTW* pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam);
        Window* pWindow = reinterpret_cast<Window*>(pCreate->lpCreateParams);

        SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWindow));
        SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&WindowProc));

        return pWindow->WindowProc(hWnd, uMsg, wParam, lParam);
    }

    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK Window::WindowProc(_In_ HWND hWnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam)
{
    switch (uMsg) {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}
