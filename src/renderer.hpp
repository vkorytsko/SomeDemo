#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <wrl.h>
#include <d3d11.h>


class Renderer
{
public:
    Renderer(HWND hWnd);
    ~Renderer() = default;

    Renderer(Renderer&&) = default;
    Renderer& operator= (Renderer&&) = default;

    Renderer(Renderer const&) = delete;
    Renderer& operator= (Renderer const&) = delete;

    void Render();
private:
    Microsoft::WRL::ComPtr<ID3D11Device> m_d3dDevice;
    Microsoft::WRL::ComPtr<IDXGISwapChain> m_swapChain;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_d3dContext;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_renderTargetView;
};
