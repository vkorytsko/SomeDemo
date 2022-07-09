#include "window.hpp"

#include <cassert>
#include <stdexcept>

#include "application.hpp"
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



Window::Window(Application* pApp, uint16_t width, uint16_t height, const std::wstring name)
    : m_width(width)
    , m_height(height)
{
    RECT windowRect = { 0, 0, m_width, m_height };
    WIN_THROW_IF_FAILED(AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, false));

    m_hWnd = CreateWindow(
        WindowClass::GetName(), name.c_str(),
        WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
        windowRect.right - windowRect.left, windowRect.bottom - windowRect.top,
        nullptr, nullptr, WindowClass::GetInstance(), pApp);

    if (!m_hWnd) {
        WIN_THROW_LAST_EXCEPTION();
    }

    WIN_THROW_IF_FAILED(ShowWindow(m_hWnd, SW_SHOWDEFAULT));

    // register mouse raw input device
    RAWINPUTDEVICE rid;
    rid.usUsagePage = 0x01; // mouse page
    rid.usUsage = 0x02; // mouse usage
    rid.dwFlags = 0;
    rid.hwndTarget = nullptr;
    WIN_THROW_IF_FAILED(RegisterRawInputDevices(&rid, 1, sizeof(rid)));

    ShowCursor(false);
    CenterCursor();
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

float Window::GetWidht() const
{
    RECT rect;
    WIN_THROW_IF_FAILED(GetClientRect(m_hWnd, &rect));

    return static_cast<float>(rect.right - rect.left);
}

float Window::GetHeight() const
{
    RECT rect;
    WIN_THROW_IF_FAILED(GetClientRect(m_hWnd, &rect));

    return static_cast<float>(rect.bottom - rect.top);
}

POINT Window::GetCenter() const
{
    RECT rect;
    WIN_THROW_IF_FAILED(GetClientRect(m_hWnd, &rect));
    const auto x = static_cast<int>((rect.right - rect.left) / 2.0f);
    const auto y = static_cast<int>((rect.bottom - rect.top) / 2.0f);

    return {x, y};
}

void Window::ShowCursor(bool show) const
{
    if (show)
    {
        while (::ShowCursor(TRUE) < 0);
    }
    else
    {
        while (::ShowCursor(FALSE) >= 0);
    }
}

void Window::CenterCursor() const
{
    const auto [x, y] = GetCenter();
    SetCursorPos(static_cast<int>(x), static_cast<int>(y));
}

LRESULT CALLBACK Window::WindowSetupProc(_In_ HWND hWnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam)
{
    switch (uMsg)
    {
        case WM_NCCREATE:
        {
            const CREATESTRUCTW* pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam);
            Application* const pApp = reinterpret_cast<Application*>(pCreate->lpCreateParams);

            SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pApp));
            SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&WindowRedirectProc));

            return pApp->WindowProc(hWnd, uMsg, wParam, lParam);
        }
        default:
            return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }
}

LRESULT CALLBACK Window::WindowRedirectProc(_In_ HWND hWnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam)
{
    switch (uMsg) {
        case WM_DESTROY:
        {
            PostQuitMessage(0);
            return 0;
        }
        default:
        {
            Application* const pApp = reinterpret_cast<Application*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
            return pApp->WindowProc(hWnd, uMsg, wParam, lParam);
        }
    }
}
