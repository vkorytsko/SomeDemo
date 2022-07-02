#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <wrl.h>
#include <d3d11.h>
#include <DirectXMath.h>

#include "debug_layer.hpp"

namespace wrl = Microsoft::WRL;
namespace dx = DirectX;


class Renderer
{
public:
    struct Vertex
    {
        dx::XMFLOAT3 pos;
        dx::XMFLOAT2 uv;
    };

    Renderer();
    ~Renderer() = default;

    Renderer(Renderer&&) = default;
    Renderer& operator= (Renderer&&) = default;

    Renderer(Renderer const&) = delete;
    Renderer& operator= (Renderer const&) = delete;

    void Update(float dt);
    void Render();
private:
    void SetupScene();
    void DrawScene();

    wrl::ComPtr<ID3D11Device> m_pD3dDevice;
    wrl::ComPtr<IDXGISwapChain> m_pSwapChain;
    wrl::ComPtr<ID3D11DeviceContext> m_pD3dContext;
    wrl::ComPtr<ID3D11RenderTargetView> m_pRenderTargetView;
    wrl::ComPtr<ID3D11DepthStencilView> m_pDepthStencilView;

    // Scene data
    wrl::ComPtr<ID3D11Buffer> m_pVertexBuffer;
    wrl::ComPtr<ID3D11Buffer> m_pIndexBuffer;
    wrl::ComPtr<ID3D11VertexShader> m_pVertexShader;
    wrl::ComPtr<ID3D11PixelShader> m_pPixelShader;
    wrl::ComPtr<ID3D11InputLayout> m_pInputLayout;
    wrl::ComPtr<ID3D11ShaderResourceView> m_pTextureView;
    wrl::ComPtr<ID3D11SamplerState> m_pSampler;

    float m_yaw = 0.0f;
    float m_pitch = 0.0f;

#ifndef NDEBUG
    DebugLayer m_debugLayer;
#endif
};
