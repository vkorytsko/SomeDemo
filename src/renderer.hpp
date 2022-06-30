#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <wrl.h>
#include <d3d11.h>

#include "debug_layer.hpp"

namespace wrl = Microsoft::WRL;


class Renderer
{
public:
    struct Vertex
    {
        struct {
            float x;
            float y;
        } pos;
        struct {
            unsigned char r;
            unsigned char g;
            unsigned char b;
            unsigned char a;
        } color;
    };

    Renderer(HWND hWnd);
    ~Renderer() = default;

    Renderer(Renderer&&) = default;
    Renderer& operator= (Renderer&&) = default;

    Renderer(Renderer const&) = delete;
    Renderer& operator= (Renderer const&) = delete;

    void Render();
private:
    void SetupScene();
    void DrawScene();

    HWND m_hWnd;

    wrl::ComPtr<ID3D11Device> m_pD3dDevice;
    wrl::ComPtr<IDXGISwapChain> m_pSwapChain;
    wrl::ComPtr<ID3D11DeviceContext> m_pD3dContext;
    wrl::ComPtr<ID3D11RenderTargetView> m_pRenderTargetView;

    // Scene data
    wrl::ComPtr<ID3D11Buffer> m_pVertexBuffer;
    wrl::ComPtr<ID3D11Buffer> m_pIndexBuffer;
    wrl::ComPtr<ID3D11VertexShader> m_pVertexShader;
    wrl::ComPtr<ID3D11PixelShader> m_pPixelShader;
    wrl::ComPtr<ID3D11InputLayout> m_pInputLayout;

#ifndef NDEBUG
    DebugLayer debugLayer;
#endif
};
