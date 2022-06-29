#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <wrl.h>
#include <d3d11.h>

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

    wrl::ComPtr<ID3D11Device> m_d3dDevice;
    wrl::ComPtr<IDXGISwapChain> m_swapChain;
    wrl::ComPtr<ID3D11DeviceContext> m_d3dContext;
    wrl::ComPtr<ID3D11RenderTargetView> m_renderTargetView;

    // Scene data
    wrl::ComPtr<ID3D11Buffer> m_vertexBuffer;
    wrl::ComPtr<ID3D11VertexShader> m_vertexShader;
    wrl::ComPtr<ID3D11PixelShader> m_pixelShader;
    wrl::ComPtr<ID3D11InputLayout> m_inputLayout;
};
