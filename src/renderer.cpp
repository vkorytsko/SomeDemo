#include "renderer.hpp"

#include <d3dcompiler.h>
#include <array>


const float EMPTY_COLOR[] = { 0.69f, 0.04f, 0.41f, 1.0f };


Renderer::Renderer(HWND hWnd)
    : m_hWnd(hWnd)
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
    sd.OutputWindow = m_hWnd;
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
    wrl::ComPtr<ID3D11Resource> pBackBuffer;
    m_swapChain->GetBuffer(0, __uuidof(ID3D11Resource), &pBackBuffer);
    m_d3dDevice->CreateRenderTargetView(
        pBackBuffer.Get(),
        nullptr,
        &m_renderTargetView
    );

    SetupScene();
}

void Renderer::Render()
{
    m_d3dContext->ClearRenderTargetView(m_renderTargetView.Get(), EMPTY_COLOR);

    DrawScene();

    // The first argument instructs DXGI to block until VSync, putting the application
    // to sleep until the next VSync. This ensures we don't waste any cycles rendering
    // frames that will never be displayed to the screen.
    m_swapChain->Present(1u, 0u);
}

void Renderer::SetupScene()
{
    // create vertex buffer (1 2d triangle at center of screen)
    const Vertex vertices[] =
    {
        { 0.0f, 0.5f, 255, 0, 0, 255 },
        { 0.5f, -0.5f, 0, 255, 0, 255 },
        { -0.5f, -0.5f, 0, 0, 255, 255 },
    };

    D3D11_BUFFER_DESC bd = {};
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.Usage = D3D11_USAGE_IMMUTABLE;
    bd.CPUAccessFlags = 0u;
    bd.MiscFlags = 0u;
    bd.ByteWidth = sizeof(vertices);
    bd.StructureByteStride = sizeof(Vertex);
    D3D11_SUBRESOURCE_DATA sd = {};
    sd.pSysMem = vertices;
    m_d3dDevice->CreateBuffer(&bd, &sd, &m_vertexBuffer);

    // create shaders
    wrl::ComPtr<ID3DBlob> pBlob;
    D3DReadFileToBlob(L"Debug/pixel.cso", &pBlob);
    m_d3dDevice->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &m_pixelShader);
    D3DReadFileToBlob(L"Debug/vertex.cso", &pBlob);
    m_d3dDevice->CreateVertexShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &m_vertexShader);

    // input (vertex) layout
    const D3D11_INPUT_ELEMENT_DESC ied[] =
    {
        { "Position", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "Color", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
    m_d3dDevice->CreateInputLayout(
        ied, static_cast<UINT>(std::size(ied)),
        pBlob->GetBufferPointer(), pBlob->GetBufferSize(),
        &m_inputLayout
    );
}

void Renderer::DrawScene()
{
    // Bind vertex buffer to pipeline
    const UINT stride = sizeof(Vertex);
    const UINT offset = 0u;
    m_d3dContext->IASetVertexBuffers(0u, 1u, m_vertexBuffer.GetAddressOf(), &stride, &offset);

    // bind shaders
    m_d3dContext->VSSetShader(m_vertexShader.Get(), nullptr, 0u);
    m_d3dContext->PSSetShader(m_pixelShader.Get(), nullptr, 0u);

    // bind vertex layout
    m_d3dContext->IASetInputLayout(m_inputLayout.Get());

    // bind render target
    m_d3dContext->OMSetRenderTargets(1u, m_renderTargetView.GetAddressOf(), nullptr);

    // Set primitive topology to triangle list (groups of 3 vertices)
    m_d3dContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // configure viewport
    RECT rect;
    GetWindowRect(m_hWnd, &rect);
    D3D11_VIEWPORT vp;
    vp.Width = static_cast<FLOAT>(rect.right - rect.left);
    vp.Height = static_cast<FLOAT>(rect.bottom - rect.top);
    vp.MinDepth = 0;
    vp.MaxDepth = 1;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    m_d3dContext->RSSetViewports(1u, &vp);


    m_d3dContext->Draw(24u, 0u);
}
