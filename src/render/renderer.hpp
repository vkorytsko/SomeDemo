#pragma once

#include <windows.h>

#include <wrl.h>
#include <d3d11.h>

#include <memory>


namespace SD::RENDER {

class DebugLayer;

class Renderer
{
public:
    Renderer(const float width, const float height, const HWND handle);
    ~Renderer();

    Renderer(Renderer&&) = default;
    Renderer& operator= (Renderer&&) = default;

    Renderer(Renderer const&) = delete;
    Renderer& operator= (Renderer const&) = delete;

    ID3D11Device* GetDevice() const { return m_pD3dDevice.Get(); }
    IDXGISwapChain* GetSwapChain() const { return m_pSwapChain.Get(); }
    ID3D11DeviceContext* GetContext() const { return m_pD3dContext.Get(); }
    ID3D11RenderTargetView* GetRenderTargetView() const { return m_pRenderTargetView.Get(); }

    DebugLayer* GetDebugLayer() const { return m_debugLayer.get(); }

private:
    Microsoft::WRL::ComPtr<ID3D11Device> m_pD3dDevice;
    Microsoft::WRL::ComPtr<IDXGISwapChain> m_pSwapChain;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_pD3dContext;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_pRenderTargetView;

    std::unique_ptr<DebugLayer> m_debugLayer;
};

}  // end namespace SD::RENDER
