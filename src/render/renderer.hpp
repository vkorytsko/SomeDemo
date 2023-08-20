#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <wrl.h>
#include <d3d11.h>
#include <DirectXMath.h>

#include <memory>

#include "debug_layer.hpp"


namespace SD::RENDER {

const DirectX::XMFLOAT3 EMPTY_COLOR = { 0.1f, 0.1f, 0.2f };

class FrameBuffer;

class Renderer
{
public:
    Renderer();
    ~Renderer();

    Renderer(Renderer&&) = default;
    Renderer& operator= (Renderer&&) = default;

    Renderer(Renderer const&) = delete;
    Renderer& operator= (Renderer const&) = delete;

    void Begin();
    void End();

    void BeginFrame();
    void EndFrame();

    void BeginImGui() const;
    void EndImGui() const;

    void OnWindowResize();
    void OnSpaceViewportResize(const float width, const float height);

    ID3D11Device* GetDevice() const;
    ID3D11DeviceContext* GetContext() const;
    DebugLayer* GetDebugLayer() const;
    FrameBuffer* GetFrameBuffer() const;

private:
    void InitImGui() const;
    void FiniImGui() const;

private:
    Microsoft::WRL::ComPtr<ID3D11Device> m_pD3dDevice;
    Microsoft::WRL::ComPtr<IDXGISwapChain> m_pSwapChain;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_pD3dContext;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_pRenderTargetView;

    std::unique_ptr<DebugLayer> m_debugLayer;

    std::unique_ptr<FrameBuffer> m_frameBuffer = nullptr;
};

}  // end namespace SD::RENDER
