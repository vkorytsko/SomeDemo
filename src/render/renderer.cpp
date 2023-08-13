#include "renderer.hpp"

#include "application.hpp"
#include "exceptions.hpp"


namespace SD::RENDER {

Renderer::Renderer()
    : m_debugLayer(std::make_unique<DebugLayer>())
{
    D3D_DEBUG_LAYER(this);

    const auto& window = ENGINE::Application::GetApplication()->GetWindow();
    const auto width = window->GetWidht();
    const auto height = window->GetHeight();

    DXGI_SWAP_CHAIN_DESC sd = {};
    sd.BufferDesc.Width = static_cast<UINT>(width);
    sd.BufferDesc.Height = static_cast<UINT>(height);
    sd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 0;
    sd.BufferDesc.RefreshRate.Denominator = 0;
    sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.BufferCount = 1;
    sd.OutputWindow = window->GetHandle();
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    sd.Flags = 0;

    UINT deviceFlags = 0u;
    if (m_debugLayer->isInitialised())
    {
        deviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
    }

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
        m_pSwapChain.GetAddressOf(),
        m_pD3dDevice.GetAddressOf(),
        nullptr,
        m_pD3dContext.GetAddressOf()
    ));

    // gain access to texture subresource in swap chain (back buffer)
    Microsoft::WRL::ComPtr<ID3D11Resource> pBackBuffer;
    D3D_THROW_INFO_EXCEPTION(m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Resource), &pBackBuffer));
    D3D_THROW_INFO_EXCEPTION(m_pD3dDevice->CreateRenderTargetView(pBackBuffer.Get(), nullptr, m_pRenderTargetView.GetAddressOf()));

    // create depth stensil state
    D3D11_DEPTH_STENCIL_DESC dsDesc = {};
    dsDesc.DepthEnable = TRUE;
    dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilState> pDSState;
    D3D_THROW_INFO_EXCEPTION(m_pD3dDevice->CreateDepthStencilState(&dsDesc, pDSState.GetAddressOf()));

    // bind depth state
    D3D_THROW_IF_INFO(m_pD3dContext->OMSetDepthStencilState(pDSState.Get(), 1u));

    // create depth stensil texture
    Microsoft::WRL::ComPtr<ID3D11Texture2D> pDepthStencil;
    D3D11_TEXTURE2D_DESC descDepth = {};
    descDepth.Width = static_cast<UINT>(width);
    descDepth.Height = static_cast<UINT>(height);
    descDepth.MipLevels = 1u;
    descDepth.ArraySize = 1u;
    descDepth.Format = DXGI_FORMAT_D32_FLOAT;
    descDepth.SampleDesc.Count = 1u;
    descDepth.SampleDesc.Quality = 0u;
    descDepth.Usage = D3D11_USAGE_DEFAULT;
    descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    D3D_THROW_INFO_EXCEPTION(m_pD3dDevice->CreateTexture2D(&descDepth, nullptr, pDepthStencil.GetAddressOf()));

    // create view of depth stensil texture
    D3D11_DEPTH_STENCIL_VIEW_DESC descDSV = {};
    descDSV.Format = DXGI_FORMAT_D32_FLOAT;
    descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    descDSV.Texture2D.MipSlice = 0u;
    D3D_THROW_INFO_EXCEPTION(m_pD3dDevice->CreateDepthStencilView(pDepthStencil.Get(), &descDSV, m_pDepthStencilView.GetAddressOf()));

    // create shadow map texture
    Microsoft::WRL::ComPtr<ID3D11Texture2D> pShadowMap;
    D3D11_TEXTURE2D_DESC shadowMap = {};
    shadowMap.Width = static_cast<UINT>(width);
    shadowMap.Height = static_cast<UINT>(height);
    shadowMap.MipLevels = 1u;
    shadowMap.ArraySize = 1u;
    shadowMap.Format = DXGI_FORMAT_R32_TYPELESS;
    shadowMap.SampleDesc.Count = 1u;
    shadowMap.SampleDesc.Quality = 0u;
    shadowMap.Usage = D3D11_USAGE_DEFAULT;
    shadowMap.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
    D3D_THROW_INFO_EXCEPTION(m_pD3dDevice->CreateTexture2D(&shadowMap, nullptr, pShadowMap.GetAddressOf()));

    // create view of shadow map
    D3D11_DEPTH_STENCIL_VIEW_DESC descSMDSV = {};
    descSMDSV.Format = DXGI_FORMAT_D32_FLOAT;
    descSMDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    descSMDSV.Texture2D.MipSlice = 0u;
    D3D_THROW_INFO_EXCEPTION(m_pD3dDevice->CreateDepthStencilView(pShadowMap.Get(), &descSMDSV, m_pSMDepthStencilView.GetAddressOf()));

    CD3D11_SHADER_RESOURCE_VIEW_DESC descSMSRV(D3D11_SRV_DIMENSION_TEXTURE2D, DXGI_FORMAT_R32_FLOAT, 0, 1);
    D3D_THROW_INFO_EXCEPTION(m_pD3dDevice->CreateShaderResourceView(pShadowMap.Get(), &descSMSRV, m_pShadowMapSRV.GetAddressOf()));

    // configure viewport
    D3D11_VIEWPORT vp;
    vp.Width = width;
    vp.Height = height;
    vp.MinDepth = 0;
    vp.MaxDepth = 1;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    D3D_THROW_IF_INFO(m_pD3dContext->RSSetViewports(1u, &vp));
}

void Renderer::BeginFrame()
{
    D3D_DEBUG_LAYER(this);

    const float color[] = { EMPTY_COLOR.x, EMPTY_COLOR.y, EMPTY_COLOR.z, 1.0f };
    D3D_THROW_IF_INFO(m_pD3dContext->ClearRenderTargetView(m_pRenderTargetView.Get(), color));
    D3D_THROW_IF_INFO(m_pD3dContext->ClearDepthStencilView(m_pDepthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0u));

    D3D_THROW_IF_INFO(m_pD3dContext->OMSetRenderTargets(1u, m_pRenderTargetView.GetAddressOf(), m_pDepthStencilView.Get()));
}

void Renderer::EndFrame()
{
    D3D_DEBUG_LAYER(this);

    debugLayer->Set();

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

ID3D11Device* Renderer::GetDevice() const
{
    return m_pD3dDevice.Get();
}

ID3D11DeviceContext* Renderer::GetContext() const
{
    return m_pD3dContext.Get();
}

DebugLayer* Renderer::GetDebugLayer() const
{
    return m_debugLayer.get();
}

}  // end namespace SD::RENDER
