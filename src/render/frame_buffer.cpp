#include "frame_buffer.hpp"

#include "renderer.hpp"
#include "debug_layer.hpp"
#include <exceptions.hpp>


namespace SD::RENDER {

FrameBuffer::FrameBuffer(Renderer* renderer, const float width, const float height, DXGI_FORMAT format)
	: m_width(static_cast<UINT>(width))
	, m_height(static_cast<UINT>(height))
    , m_format(format)
{
    createTextures(renderer);
    createViews(renderer);
    createStates(renderer);
}

void FrameBuffer::bind(Renderer* renderer, bool depth) const
{
    D3D_DEBUG_LAYER(renderer);

    // bind depth state
    if (depth)
    {
        D3D_THROW_IF_INFO(renderer->GetContext()->OMSetDepthStencilState(m_pDepthStencilStateEnabled.Get(), 1u));
    }
    else
    {
        D3D_THROW_IF_INFO(renderer->GetContext()->OMSetDepthStencilState(m_pDepthStencilStateDisabled.Get(), 1u));
    }
}

void FrameBuffer::resize(Renderer* renderer, const UINT width, const UINT height)
{
    m_width = width;
    m_height = height;

    m_pDepthStencilStateEnabled->Release();
    m_pDepthStencilStateDisabled->Release();

    m_pRenderTargetView->Release();
    m_pShaderResourceView->Release();
    m_pDepthStencilView->Release();

    m_pRenderTarget->Release();
    m_pDepthStencil->Release();

    createTextures(renderer);
    createViews(renderer);
    createStates(renderer);
}

void FrameBuffer::createTextures(const Renderer* renderer)
{
    D3D_DEBUG_LAYER(renderer);

    // create frame buffer texture
    D3D11_TEXTURE2D_DESC renderTargetTextureDesc = {};
    renderTargetTextureDesc.Width = m_width;
    renderTargetTextureDesc.Height = m_height;
    renderTargetTextureDesc.MipLevels = 1;
    renderTargetTextureDesc.ArraySize = 1;
    renderTargetTextureDesc.Format = m_format;
    renderTargetTextureDesc.SampleDesc.Count = 1;
    renderTargetTextureDesc.SampleDesc.Quality = 0;
    renderTargetTextureDesc.Usage = D3D11_USAGE_DEFAULT;
    renderTargetTextureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
    renderTargetTextureDesc.CPUAccessFlags = 0;
    renderTargetTextureDesc.MiscFlags = 0;
    D3D_THROW_INFO_EXCEPTION(renderer->GetDevice()->CreateTexture2D(&renderTargetTextureDesc, nullptr, m_pRenderTarget.GetAddressOf()));

    // create depth stencil texture
    D3D11_TEXTURE2D_DESC depthStencilTextureDesc = {};
    depthStencilTextureDesc.Width = m_width;
    depthStencilTextureDesc.Height = m_height;
    depthStencilTextureDesc.MipLevels = 1u;
    depthStencilTextureDesc.ArraySize = 1u;
    depthStencilTextureDesc.Format = DXGI_FORMAT_D32_FLOAT;
    depthStencilTextureDesc.SampleDesc.Count = 1u;
    depthStencilTextureDesc.SampleDesc.Quality = 0u;
    depthStencilTextureDesc.Usage = D3D11_USAGE_DEFAULT;
    depthStencilTextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    D3D_THROW_INFO_EXCEPTION(renderer->GetDevice()->CreateTexture2D(&depthStencilTextureDesc, nullptr, m_pDepthStencil.GetAddressOf()));
}

void FrameBuffer::createViews(const Renderer* renderer)
{
    D3D_DEBUG_LAYER(renderer);

    D3D11_TEXTURE2D_DESC renderTargetTextureDesc;
    m_pRenderTarget->GetDesc(&renderTargetTextureDesc);
    D3D11_TEXTURE2D_DESC depthStencilTextureDesc;
    m_pDepthStencil->GetDesc(&depthStencilTextureDesc);

    // create render target view
    D3D11_RENDER_TARGET_VIEW_DESC descRTV = {};
    descRTV.Format = renderTargetTextureDesc.Format;
    descRTV.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
    D3D_THROW_INFO_EXCEPTION(renderer->GetDevice()->CreateRenderTargetView(m_pRenderTarget.Get(), &descRTV, m_pRenderTargetView.GetAddressOf()));

    // create shader resource view
    D3D11_SHADER_RESOURCE_VIEW_DESC descSRV = {};
    descSRV.Format = renderTargetTextureDesc.Format;
    descSRV.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    descSRV.Texture2D.MostDetailedMip = 0;
    descSRV.Texture2D.MipLevels = 1;
    D3D_THROW_INFO_EXCEPTION(renderer->GetDevice()->CreateShaderResourceView(m_pRenderTarget.Get(), &descSRV, m_pShaderResourceView.GetAddressOf()));

    // create view of depth stencil texture
    D3D11_DEPTH_STENCIL_VIEW_DESC descDSV = {};
    descDSV.Format = depthStencilTextureDesc.Format;
    descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    descDSV.Texture2D.MipSlice = 0u;
    D3D_THROW_INFO_EXCEPTION(renderer->GetDevice()->CreateDepthStencilView(m_pDepthStencil.Get(), &descDSV, m_pDepthStencilView.GetAddressOf()));
}

void FrameBuffer::createStates(const Renderer* renderer)
{
    D3D_DEBUG_LAYER(renderer);

    // create depth stencil states
    D3D11_DEPTH_STENCIL_DESC depthStencilDesc = {};
    depthStencilDesc.DepthEnable = FALSE;
    D3D_THROW_INFO_EXCEPTION(renderer->GetDevice()->CreateDepthStencilState(&depthStencilDesc, m_pDepthStencilStateDisabled.GetAddressOf()));

    depthStencilDesc.DepthEnable = TRUE;
    depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
    D3D_THROW_INFO_EXCEPTION(renderer->GetDevice()->CreateDepthStencilState(&depthStencilDesc, m_pDepthStencilStateEnabled.GetAddressOf()));
}


CubeFrameBuffer::CubeFrameBuffer(Renderer* renderer, const float size, const uint8_t mips)
	: m_size(static_cast<UINT>(size))
    , m_mips(static_cast<UINT>(mips))
{
    createTextures(renderer);
    createViews(renderer);
    createStates(renderer);
}

void CubeFrameBuffer::bind(Renderer* renderer) const
{
    D3D_DEBUG_LAYER(renderer);

    // bind depth state
    D3D_THROW_IF_INFO(renderer->GetContext()->OMSetDepthStencilState(m_pDepthStencilState.Get(), 1u));
}

void CubeFrameBuffer::resize(Renderer* renderer, const UINT size)
{
    m_size = size;

    m_pDepthStencilState->Release();

    for (uint8_t face = 0u; face < FACE_COUNT; ++face)
    {
        for (uint8_t mip = 0u; mip < m_mips; ++mip)
        {
            m_pRenderTargetView[face][mip]->Release();
            m_pDepthStencilView[face][mip]->Release();
        }
    }

	m_pShaderResourceView->Release();

    m_pRenderTarget->Release();
    m_pDepthStencil->Release();

    createTextures(renderer);
    createViews(renderer);
    createStates(renderer);
}

void CubeFrameBuffer::createTextures(const Renderer* renderer)
{
    D3D_DEBUG_LAYER(renderer);

    // create frame buffer texture
    D3D11_TEXTURE2D_DESC renderTargetTextureDesc = {};
    renderTargetTextureDesc.Width = m_size;
    renderTargetTextureDesc.Height = m_size;
    renderTargetTextureDesc.MipLevels = m_mips == 0 ? -1 : m_mips;
    renderTargetTextureDesc.ArraySize = 6;
    renderTargetTextureDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    renderTargetTextureDesc.SampleDesc.Count = 1;
    renderTargetTextureDesc.SampleDesc.Quality = 0;
    renderTargetTextureDesc.Usage = D3D11_USAGE_DEFAULT;
    renderTargetTextureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
    renderTargetTextureDesc.CPUAccessFlags = 0;
    renderTargetTextureDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;
    D3D_THROW_INFO_EXCEPTION(renderer->GetDevice()->CreateTexture2D(&renderTargetTextureDesc, nullptr, m_pRenderTarget.GetAddressOf()));

    // create depth stencil texture
    D3D11_TEXTURE2D_DESC depthStencilTextureDesc = {};
    depthStencilTextureDesc.Width = m_size;
    depthStencilTextureDesc.Height = m_size;
    depthStencilTextureDesc.MipLevels = m_mips == 0 ? -1 : m_mips;
    depthStencilTextureDesc.ArraySize = 6u;
    depthStencilTextureDesc.Format = DXGI_FORMAT_D32_FLOAT;
    depthStencilTextureDesc.SampleDesc.Count = 1u;
    depthStencilTextureDesc.SampleDesc.Quality = 0u;
    depthStencilTextureDesc.Usage = D3D11_USAGE_DEFAULT;
    depthStencilTextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    D3D_THROW_INFO_EXCEPTION(renderer->GetDevice()->CreateTexture2D(&depthStencilTextureDesc, nullptr, m_pDepthStencil.GetAddressOf()));
}

void CubeFrameBuffer::createViews(const Renderer* renderer)
{
    D3D_DEBUG_LAYER(renderer);

    D3D11_TEXTURE2D_DESC renderTargetTextureDesc;
    m_pRenderTarget->GetDesc(&renderTargetTextureDesc);
    D3D11_TEXTURE2D_DESC depthStencilTextureDesc;
    m_pDepthStencil->GetDesc(&depthStencilTextureDesc);

    // create render target views
    for (uint8_t face = 0u; face < FACE_COUNT; ++face)
    {
        m_pRenderTargetView[face].resize(m_mips);
        for (uint8_t mip = 0u; mip < m_mips; ++mip)
        {
            D3D11_RENDER_TARGET_VIEW_DESC descRTV = {};
            descRTV.Format = renderTargetTextureDesc.Format;
            descRTV.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
            descRTV.Texture2DArray.FirstArraySlice = face;
            descRTV.Texture2DArray.ArraySize = 1;
            descRTV.Texture2DArray.MipSlice = mip;
            D3D_THROW_INFO_EXCEPTION(renderer->GetDevice()->CreateRenderTargetView(m_pRenderTarget.Get(), &descRTV, m_pRenderTargetView[face][mip].GetAddressOf()));
        }
    }

    // create shader resource view
    D3D11_SHADER_RESOURCE_VIEW_DESC descSRV = {};
    descSRV.Format = renderTargetTextureDesc.Format;
    descSRV.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
    descSRV.TextureCube.MostDetailedMip = 0;
    descSRV.TextureCube.MipLevels = renderTargetTextureDesc.MipLevels;
    D3D_THROW_INFO_EXCEPTION(renderer->GetDevice()->CreateShaderResourceView(m_pRenderTarget.Get(), &descSRV, m_pShaderResourceView.GetAddressOf()));

    // create depth stencil views
    for (uint8_t face = 0u; face < FACE_COUNT; ++face)
    {
        m_pDepthStencilView[face].resize(m_mips);
        for (uint8_t mip = 0u; mip < m_mips; ++mip)
        {
            D3D11_DEPTH_STENCIL_VIEW_DESC descDSV = {};
            descDSV.Format = depthStencilTextureDesc.Format;
            descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
            descDSV.Texture2DArray.MipSlice = mip;
            descDSV.Texture2DArray.FirstArraySlice = face;
            descDSV.Texture2DArray.ArraySize = 1;
            D3D_THROW_INFO_EXCEPTION(renderer->GetDevice()->CreateDepthStencilView(m_pDepthStencil.Get(), &descDSV, m_pDepthStencilView[face][mip].GetAddressOf()));
        }
    }
}

void CubeFrameBuffer::createStates(const Renderer* renderer)
{
    D3D_DEBUG_LAYER(renderer);

    // create depth stencil state
    D3D11_DEPTH_STENCIL_DESC depthStencilDesc = {};
    depthStencilDesc.DepthEnable = TRUE;
    depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
    D3D_THROW_INFO_EXCEPTION(renderer->GetDevice()->CreateDepthStencilState(&depthStencilDesc, m_pDepthStencilState.GetAddressOf()));
}
}  // end namespace SD::RENDER
