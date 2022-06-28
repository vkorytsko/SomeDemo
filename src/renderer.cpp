#include "renderer.hpp"

const float EMPTY_COLOR[] = { 0.69f, 0.04f, 0.41f, 1.0f };


Renderer::Renderer(HWND hWnd)
{
    DXGI_SWAP_CHAIN_DESC sd = {};
    sd.BufferDesc.Width = 0;
    sd.BufferDesc.Height = 0;
    sd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 0;
    sd.BufferDesc.RefreshRate.Denominator = 0;
    sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.BufferCount = 1;
    sd.OutputWindow = hWnd;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    sd.Flags = 0;

    // create device and front/back buffers, and swap chain and rendering context
    D3D11CreateDeviceAndSwapChain(
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        0,
        nullptr,
        0,
        D3D11_SDK_VERSION,
        &sd,
        &m_swapChain,
        &m_d3dDevice,
        nullptr,
        &m_d3dContext
    );
    // gain access to texture subresource in swap chain (back buffer)
    Microsoft::WRL::ComPtr<ID3D11Resource> pBackBuffer;
    m_swapChain->GetBuffer(0, __uuidof(ID3D11Resource), &pBackBuffer);
    m_d3dDevice->CreateRenderTargetView(
        pBackBuffer.Get(),
        nullptr,
        &m_renderTargetView
    );
}

void Renderer::Render()
{
    m_d3dContext->ClearRenderTargetView(m_renderTargetView.Get(), EMPTY_COLOR);

    // The first argument instructs DXGI to block until VSync, putting the application
    // to sleep until the next VSync. This ensures we don't waste any cycles rendering
    // frames that will never be displayed to the screen.
    m_swapChain->Present(1u, 0u);
}
