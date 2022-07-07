#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <wrl.h>
#include <d3d11.h>
#include <DirectXMath.h>

#include <memory>

#include "debug_layer.hpp"

namespace wrl = Microsoft::WRL;
namespace dx = DirectX;

const dx::XMFLOAT3 EMPTY_COLOR = { 0.1f, 0.1f, 0.2f };


class Renderer
{
public:
    Renderer();
    ~Renderer() = default;

    Renderer(Renderer&&) = default;
    Renderer& operator= (Renderer&&) = default;

    Renderer(Renderer const&) = delete;
    Renderer& operator= (Renderer const&) = delete;

    void BeginFrame();
    void EndFrame();

    ID3D11Device* GetDevice() const;
    ID3D11DeviceContext* GetContext() const;
    DebugLayer* GetDebugLayer() const;
private:
    wrl::ComPtr<ID3D11Device> m_pD3dDevice;
    wrl::ComPtr<IDXGISwapChain> m_pSwapChain;
    wrl::ComPtr<ID3D11DeviceContext> m_pD3dContext;
    wrl::ComPtr<ID3D11RenderTargetView> m_pRenderTargetView;
    wrl::ComPtr<ID3D11DepthStencilView> m_pDepthStencilView;


    std::unique_ptr<DebugLayer> m_debugLayer;

};
