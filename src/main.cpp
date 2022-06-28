#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <stdexcept>

#include "renderer.hpp"


LPCWSTR WINDOW_CLASS_NAME = L"Some window class";
LPCWSTR APP_NAME = L"Some Demo";
const uint32_t WIDTH = 900;
const uint32_t HEIGHT = 600;


LRESULT CALLBACK WindowProc(_In_ HWND hwnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam);


int main(int, char**)
{
    // Register class and create window
    HINSTANCE hInstance = GetModuleHandle(nullptr);

    WNDCLASS windowClass = {};
    windowClass.lpfnWndProc = WindowProc;
    windowClass.hInstance = hInstance;
    windowClass.lpszClassName = WINDOW_CLASS_NAME;

    if (FAILED(RegisterClass(&windowClass))) {
        throw std::runtime_error("Failed to register Window Class!");
    }

    RECT windowRect = { 0, 0, WIDTH, 600 };
    AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, false);

    HWND hWnd = CreateWindow(
        WINDOW_CLASS_NAME, APP_NAME,
        WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
        windowRect.right - windowRect.left, windowRect.bottom - windowRect.top,
        nullptr, nullptr, hInstance, nullptr
    );

    if (!hWnd) {
        throw std::runtime_error("Failed to create Window!");
    }

    ShowWindow(hWnd, SW_SHOWDEFAULT);

    Renderer renderer{hWnd};

    MSG msg = {};
    while (msg.message != WM_QUIT)
    {
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        renderer.Render();
    }

    return static_cast<int>(msg.wParam);
}

LRESULT CALLBACK WindowProc(_In_ HWND hWnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam)
{

    switch (uMsg) {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

