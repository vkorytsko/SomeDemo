#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <memory>
#include <optional>


class Window
{
private:
    class WindowClass
    {
    public:
        static const wchar_t* GetName() noexcept;
        static HINSTANCE GetInstance() noexcept;
    private:
        WindowClass();
        ~WindowClass();
        WindowClass(const WindowClass&) = delete;
        WindowClass& operator=(const WindowClass&) = delete;

    private:
        static constexpr const wchar_t* m_wndClassName = L"Some Demo window class";
        static WindowClass m_wndClass;

        HINSTANCE m_hInstance = nullptr;
    };

public:
    Window(int width, int height, const wchar_t* name);
    ~Window();
    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;

    std::optional<int> ProcessMessages();
    HWND GetHandle() const;

private:
    static LRESULT CALLBACK WindowSetupProc(_In_ HWND hwnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam);
    static LRESULT CALLBACK WindowProc(_In_ HWND hwnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam);

private:
    int m_width;
    int m_height;

    HWND m_hWnd = nullptr;
};
