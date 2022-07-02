#include "renderer.hpp"

#include <d3dcompiler.h>
#include <DirectXTex.h>
#include <array>

#include "exceptions.hpp"


const float EMPTY_COLOR[] = { 0.69f, 0.04f, 0.41f, 1.0f };

dx::ScratchImage loadImage(const wchar_t* name);


Renderer::Renderer(HWND hWnd, const uint16_t width, const uint16_t height)
    : m_hWnd(hWnd)
{
    DXGI_SWAP_CHAIN_DESC sd = {};
    sd.BufferDesc.Width = width;
    sd.BufferDesc.Height = height;
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
    if (m_debugLayer.isInitialised())
    {
        deviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
    }
#endif

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

    // create depth stensil state
    D3D11_DEPTH_STENCIL_DESC dsDesc = {};
    dsDesc.DepthEnable = TRUE;
    dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
    wrl::ComPtr<ID3D11DepthStencilState> pDSState;
    D3D_THROW_INFO_EXCEPTION(m_pD3dDevice->CreateDepthStencilState(&dsDesc, &pDSState));

    // bind depth state
    D3D_THROW_IF_INFO(m_pD3dContext->OMSetDepthStencilState(pDSState.Get(), 1u));

    // create depth stensil texture
    wrl::ComPtr<ID3D11Texture2D> pDepthStencil;
    D3D11_TEXTURE2D_DESC descDepth = {};
    descDepth.Width = width;
    descDepth.Height = height;
    descDepth.MipLevels = 1u;
    descDepth.ArraySize = 1u;
    descDepth.Format = DXGI_FORMAT_D32_FLOAT;
    descDepth.SampleDesc.Count = 1u;
    descDepth.SampleDesc.Quality = 0u;
    descDepth.Usage = D3D11_USAGE_DEFAULT;
    descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    D3D_THROW_INFO_EXCEPTION(m_pD3dDevice->CreateTexture2D(&descDepth, nullptr, &pDepthStencil));

    // create view of depth stensil texture
    D3D11_DEPTH_STENCIL_VIEW_DESC descDSV = {};
    descDSV.Format = DXGI_FORMAT_D32_FLOAT;
    descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    descDSV.Texture2D.MipSlice = 0u;
    D3D_THROW_INFO_EXCEPTION(m_pD3dDevice->CreateDepthStencilView(pDepthStencil.Get(), &descDSV, &m_pDepthStencilView));

    // bind render target and depth stensil view to OM
    D3D_THROW_IF_INFO(m_pD3dContext->OMSetRenderTargets(1u, m_pRenderTargetView.GetAddressOf(), m_pDepthStencilView.Get()));

    // configure viewport
    D3D11_VIEWPORT vp;
    vp.Width = width;
    vp.Height = height;
    vp.MinDepth = 0;
    vp.MaxDepth = 1;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    m_pD3dContext->RSSetViewports(1u, &vp);

    SetupScene();
}

void Renderer::Update(float dt)
{
    m_yaw += dt;
    m_pitch += dt;
}

void Renderer::Render()
{
    D3D_THROW_IF_INFO(m_pD3dContext->ClearRenderTargetView(m_pRenderTargetView.Get(), EMPTY_COLOR));
    D3D_THROW_IF_INFO(m_pD3dContext->ClearDepthStencilView(m_pDepthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0u));

    DrawScene();

#ifndef NDEBUG
    m_debugLayer.Set();
#endif

    HRESULT hr;
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
    // create vertex buffer
    const Vertex vertices[] =
    {
        {{-1.0f, -1.0f, -1.0f}, {2.0f / 3.0f, 0.0f / 4.0f}},
        {{ 1.0f, -1.0f, -1.0f}, {1.0f / 3.0f ,0.0f / 4.0f}},
        {{-1.0f,  1.0f, -1.0f}, {2.0f / 3.0f, 1.0f / 4.0f}},
        {{ 1.0f,  1.0f, -1.0f}, {1.0f / 3.0f, 1.0f / 4.0f}},
        {{-1.0f, -1.0f,  1.0f}, {2.0f / 3.0f, 3.0f / 4.0f}},
        {{ 1.0f, -1.0f,  1.0f}, {1.0f / 3.0f, 3.0f / 4.0f}},
        {{-1.0f,  1.0f,  1.0f}, {2.0f / 3.0f, 2.0f / 4.0f}},
        {{ 1.0f,  1.0f,  1.0f}, {1.0f / 3.0f, 2.0f / 4.0f}},
        {{-1.0f, -1.0f, -1.0f}, {2.0f / 3.0f, 4.0f / 4.0f}},
        {{ 1.0f, -1.0f, -1.0f}, {1.0f / 3.0f, 4.0f / 4.0f}},
        {{-1.0f, -1.0f, -1.0f}, {3.0f / 3.0f, 1.0f / 4.0f}},
        {{-1.0f, -1.0f,  1.0f}, {3.0f / 3.0f, 2.0f / 4.0f}},
        {{ 1.0f, -1.0f, -1.0f}, {0.0f / 3.0f, 1.0f / 4.0f}},
        {{ 1.0f, -1.0f,  1.0f}, {0.0f / 3.0f, 2.0f / 4.0f}},
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
        0,  2,  1,   2,  3,  1,
        4,  8,  5,   5,  8,  9,
        2,  6,  3,   3,  6,  7,
        4,  5,  7,   4,  7,  6,
        2,  10, 11,  2,  11, 6,
        12, 3,  7,   12, 7,  13,
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
    D3D_THROW_INFO_EXCEPTION(D3DReadFileToBlob(L"Debug/texture.ps.cso", &pBlob));
    D3D_THROW_INFO_EXCEPTION(m_pD3dDevice->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &m_pPixelShader));
    D3D_THROW_INFO_EXCEPTION(D3DReadFileToBlob(L"Debug/texture.vs.cso", &pBlob));
    D3D_THROW_INFO_EXCEPTION(m_pD3dDevice->CreateVertexShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &m_pVertexShader));

    // input (vertex) layout
    const D3D11_INPUT_ELEMENT_DESC ied[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
    D3D_THROW_INFO_EXCEPTION(m_pD3dDevice->CreateInputLayout(
        ied, static_cast<UINT>(std::size(ied)),
        pBlob->GetBufferPointer(), pBlob->GetBufferSize(),
        &m_pInputLayout
    ));

    dx::ScratchImage scratch = loadImage(L"../res/textures/cube.png");
    const auto textureWidth = static_cast<UINT>(scratch.GetMetadata().width);
    const auto textureHeight = static_cast<UINT>(scratch.GetMetadata().height);
    const auto rowPitch = static_cast<UINT>(scratch.GetImage(0, 0, 0)->rowPitch);

    // create texture resource
    D3D11_TEXTURE2D_DESC td = {};
    td.Width = textureWidth;
    td.Height = textureHeight;
    td.MipLevels = 1;
    td.ArraySize = 1;
    td.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    td.SampleDesc.Count = 1;
    td.SampleDesc.Quality = 0;
    td.Usage = D3D11_USAGE_IMMUTABLE;
    td.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    td.CPUAccessFlags = 0;
    td.MiscFlags = 0;
    D3D11_SUBRESOURCE_DATA tsd = {};
    tsd.pSysMem = scratch.GetPixels();
    tsd.SysMemPitch = rowPitch;
    wrl::ComPtr<ID3D11Texture2D> pTexture;
    D3D_THROW_INFO_EXCEPTION(m_pD3dDevice->CreateTexture2D(&td, &tsd, &pTexture));

    // create the resource view on the texture
    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Format = td.Format;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MostDetailedMip = 0;
    srvDesc.Texture2D.MipLevels = 1;
    D3D_THROW_INFO_EXCEPTION(m_pD3dDevice->CreateShaderResourceView(pTexture.Get(), &srvDesc, &m_pTextureView));

    // create texture sampler
    D3D11_SAMPLER_DESC samplerDesc = {};
    samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    D3D_THROW_INFO_EXCEPTION(m_pD3dDevice->CreateSamplerState(&samplerDesc, &m_pSampler));
}

void Renderer::DrawScene()
{
    RECT rect;
    WIN_THROW_IF_FAILED(GetClientRect(m_hWnd, &rect));
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
                dx::XMMatrixRotationX(m_pitch) *
                dx::XMMatrixRotationZ(m_yaw) *
                dx::XMMatrixTranslation(0.0f, 0.0f, 5.0f) *
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

    // bind texture to pixel shader
    D3D_THROW_IF_INFO(m_pD3dContext->PSSetShaderResources(0u, 1u, m_pTextureView.GetAddressOf()));

    // bind texture sampler to pixel shader
    D3D_THROW_IF_INFO(m_pD3dContext->PSSetSamplers(0, 1, m_pSampler.GetAddressOf()));

    // bind vertex layout
    D3D_THROW_IF_INFO(m_pD3dContext->IASetInputLayout(m_pInputLayout.Get()));

    // Set primitive topology to triangle list (groups of 3 vertices)
    D3D_THROW_IF_INFO(m_pD3dContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));

    D3D_THROW_IF_INFO(m_pD3dContext->DrawIndexed(36u, 0u, 0u));
}

dx::ScratchImage loadImage(const wchar_t* name)
{
    dx::ScratchImage scratch;
    WIN_THROW_IF_FAILED(dx::LoadFromWICFile(name, dx::WIC_FLAGS_IGNORE_SRGB, nullptr, scratch));

    if (scratch.GetImage(0, 0, 0)->format != DXGI_FORMAT::DXGI_FORMAT_B8G8R8A8_UNORM)
    {
        dx::ScratchImage converted;

        WIN_THROW_IF_FAILED(dx::Convert(
            *scratch.GetImage(0, 0, 0),
            DXGI_FORMAT::DXGI_FORMAT_B8G8R8A8_UNORM,
            dx::TEX_FILTER_DEFAULT,
            dx::TEX_THRESHOLD_DEFAULT,
            converted
        ));

        return converted;
    }

    return scratch;
}
