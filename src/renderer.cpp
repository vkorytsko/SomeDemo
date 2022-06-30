#include "renderer.hpp"

#include <d3dcompiler.h>
#include <array>

#include "exceptions.hpp"


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

    UINT deviceFlags = 0u;
#ifndef NDEBUG
    deviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    // for checking results of d3d functions
    HRESULT hr;

    // create device and front/back buffers, and swap chain and rendering context
    D3D_THROW_INFO_EXCEPTION(D3D11CreateDeviceAndSwapChain(
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        deviceFlags,
        nullptr,
        0,
        D3D11_SDK_VERSION,
        &sd,
        &m_pSwapChain,
        &m_pD3dDevice,
        nullptr,
        &m_pD3dContext
    ));
    // gain access to texture subresource in swap chain (back buffer)
    wrl::ComPtr<ID3D11Resource> pBackBuffer;
    D3D_THROW_INFO_EXCEPTION(m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Resource), &pBackBuffer));
    D3D_THROW_INFO_EXCEPTION(m_pD3dDevice->CreateRenderTargetView(pBackBuffer.Get(), nullptr, &m_pRenderTargetView));

    SetupScene();
}

void Renderer::Update(float dt)
{
    m_yaw += dt;
}

void Renderer::Render()
{
    // for checking results of d3d functions
    HRESULT hr;

    m_pD3dContext->ClearRenderTargetView(m_pRenderTargetView.Get(), EMPTY_COLOR);

    DrawScene();

#ifndef NDEBUG
    debugLayer.Set();
#endif
    // First argument for VSync
    if (FAILED(hr = m_pSwapChain->Present(0u, 0u)))
    {
        if (hr == DXGI_ERROR_DEVICE_REMOVED)
        {
            throw D3D_EXCEPTION(m_pD3dDevice->GetDeviceRemovedReason());
        }
        else
        {
            throw D3D_EXCEPTION(hr);
        }
    }
}

void Renderer::SetupScene()
{
    // for checking results of d3d functions
    HRESULT hr;

    // create vertex buffer
    const Vertex vertices[] =
    {
        { 0.0f, 1.0f, 0.0f, 255, 0, 0, 255 },
        { 0.7f, -0.7f, 0.0f, 0, 255, 0, 255 },
        { -0.7f, -0.7f, 0.0f, 0, 0, 255, 255 },
    };

    D3D11_BUFFER_DESC vbd = {};
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbd.Usage = D3D11_USAGE_IMMUTABLE;
    vbd.CPUAccessFlags = 0u;
    vbd.MiscFlags = 0u;
    vbd.ByteWidth = sizeof(vertices);
    vbd.StructureByteStride = sizeof(Vertex);
    D3D11_SUBRESOURCE_DATA vsd = {};
    vsd.pSysMem = vertices;
    D3D_THROW_INFO_EXCEPTION(m_pD3dDevice->CreateBuffer(&vbd, &vsd, &m_pVertexBuffer));

    // create index buffer
    const unsigned short indices[] =
    {
        0, 1, 2,
    };

    D3D11_BUFFER_DESC ibd = {};
    ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibd.Usage = D3D11_USAGE_IMMUTABLE;
    ibd.CPUAccessFlags = 0u;
    ibd.MiscFlags = 0u;
    ibd.ByteWidth = sizeof(indices);
    ibd.StructureByteStride = sizeof(unsigned short);
    D3D11_SUBRESOURCE_DATA isd = {};
    isd.pSysMem = indices;
    D3D_THROW_INFO_EXCEPTION(m_pD3dDevice->CreateBuffer(&ibd, &isd, &m_pIndexBuffer));

    // create shaders
    wrl::ComPtr<ID3DBlob> pBlob;
    D3D_THROW_INFO_EXCEPTION(D3DReadFileToBlob(L"Debug/pixel.cso", &pBlob));
    D3D_THROW_INFO_EXCEPTION(m_pD3dDevice->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &m_pPixelShader));
    D3D_THROW_INFO_EXCEPTION(D3DReadFileToBlob(L"Debug/vertex.cso", &pBlob));
    D3D_THROW_INFO_EXCEPTION(m_pD3dDevice->CreateVertexShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &m_pVertexShader));

    // input (vertex) layout
    const D3D11_INPUT_ELEMENT_DESC ied[] =
    {
        { "Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "Color", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
    D3D_THROW_INFO_EXCEPTION(m_pD3dDevice->CreateInputLayout(
        ied, static_cast<UINT>(std::size(ied)),
        pBlob->GetBufferPointer(), pBlob->GetBufferSize(),
        &m_pInputLayout
    ));
}

void Renderer::DrawScene()
{
    // for checking results of d3d functions
    HRESULT hr;

    RECT rect;
    GetWindowRect(m_hWnd, &rect);
    const float width = static_cast<float>(rect.right - rect.left);
    const float height = static_cast<float>(rect.bottom - rect.top);

    // create constant buffer
    struct ConstantBuffer
    {
        dx::XMMATRIX transform;
    };
    const ConstantBuffer cb =
    {
        {
            dx::XMMatrixTranspose(
                dx::XMMatrixRotationZ(m_yaw) *
                dx::XMMatrixTranslation(0.0f, 0.0f, 2.0f) *
                dx::XMMatrixPerspectiveLH(1.0f, height / width, 0.5f, 10.0f)
            )
        }
    };
    wrl::ComPtr<ID3D11Buffer> pConstantBuffer;
    D3D11_BUFFER_DESC cbd;
    cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cbd.Usage = D3D11_USAGE_DYNAMIC;
    cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    cbd.MiscFlags = 0u;
    cbd.ByteWidth = sizeof(cb);
    cbd.StructureByteStride = 0u;
    D3D11_SUBRESOURCE_DATA csd = {};
    csd.pSysMem = &cb;
    D3D_THROW_INFO_EXCEPTION(m_pD3dDevice->CreateBuffer(&cbd, &csd, &pConstantBuffer));

    // Bind vertex buffer
    const UINT stride = sizeof(Vertex);
    const UINT offset = 0u;
    D3D_THROW_IF_INFO(m_pD3dContext->IASetVertexBuffers(0u, 1u, m_pVertexBuffer.GetAddressOf(), &stride, &offset));

    // Bind vertex buffer
    D3D_THROW_IF_INFO(m_pD3dContext->IASetIndexBuffer(m_pIndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0u));

    // bind shaders
    D3D_THROW_IF_INFO(m_pD3dContext->VSSetShader(m_pVertexShader.Get(), nullptr, 0u));
    D3D_THROW_IF_INFO(m_pD3dContext->PSSetShader(m_pPixelShader.Get(), nullptr, 0u));

    // bind constant buffer to vertex shader
    D3D_THROW_IF_INFO(m_pD3dContext->VSSetConstantBuffers(0u, 1u, pConstantBuffer.GetAddressOf()));

    // bind vertex layout
    D3D_THROW_IF_INFO(m_pD3dContext->IASetInputLayout(m_pInputLayout.Get()));

    // bind render target
    D3D_THROW_IF_INFO(m_pD3dContext->OMSetRenderTargets(1u, m_pRenderTargetView.GetAddressOf(), nullptr));

    // Set primitive topology to triangle list (groups of 3 vertices)
    D3D_THROW_IF_INFO(m_pD3dContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));

    // configure viewport
    D3D11_VIEWPORT vp;
    vp.Width = width;
    vp.Height = height;
    vp.MinDepth = 0;
    vp.MaxDepth = 1;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    m_pD3dContext->RSSetViewports(1u, &vp);

    D3D_THROW_IF_INFO(m_pD3dContext->DrawIndexed(3u, 0u, 0u));
}
