#include "frame_buffer.hpp"

#include "renderer.hpp"
#include "debug_layer.hpp"
#include <exceptions.hpp>


namespace SD::RENDER {

FrameBuffer::FrameBuffer(Renderer* renderer, const float width, const float height)
	: m_width(static_cast<UINT>(width))
	, m_height(static_cast<UINT>(height))
{
    D3D_DEBUG_LAYER(renderer);

    createViews(renderer);

    // create depth stencil state
    D3D11_DEPTH_STENCIL_DESC dsDesc = {};
    dsDesc.DepthEnable = TRUE;
    dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilState> pDSState;
    D3D_THROW_INFO_EXCEPTION(renderer->GetDevice()->CreateDepthStencilState(&dsDesc, pDSState.GetAddressOf()));

    // bind depth state
    D3D_THROW_IF_INFO(renderer->GetContext()->OMSetDepthStencilState(pDSState.Get(), 1u));
}

Microsoft::WRL::ComPtr<ID3D11RenderTargetView> FrameBuffer::getRTV() const
{
    return m_pRenderTargetView;
}

Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> FrameBuffer::getSRV() const
{
	return m_pShaderResourceView;
}

Microsoft::WRL::ComPtr<ID3D11DepthStencilView> FrameBuffer::getDSV() const
{
	return m_pDepthStencilView;
}

void FrameBuffer::resize(Renderer* renderer, const UINT width, const UINT height)
{
    m_width = width;
    m_height = height;

    m_pRenderTargetView->Release();
    m_pShaderResourceView->Release();
    m_pDepthStencilView->Release();

    createViews(renderer);
}

void FrameBuffer::createViews(Renderer* renderer)
{
    D3D_DEBUG_LAYER(renderer);

    // create frame buffer texture
    D3D11_TEXTURE2D_DESC frameBufferTextureDesc = {};
    frameBufferTextureDesc.Width = m_width;
    frameBufferTextureDesc.Height = m_height;
    frameBufferTextureDesc.MipLevels = 1;
    frameBufferTextureDesc.ArraySize = 1;
    frameBufferTextureDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    frameBufferTextureDesc.SampleDesc.Count = 1;
    frameBufferTextureDesc.SampleDesc.Quality = 0;
    frameBufferTextureDesc.Usage = D3D11_USAGE_DEFAULT;
    frameBufferTextureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
    frameBufferTextureDesc.CPUAccessFlags = 0;
    frameBufferTextureDesc.MiscFlags = 0;

    Microsoft::WRL::ComPtr<ID3D11Texture2D> pFrameBuffer;
    D3D_THROW_INFO_EXCEPTION(renderer->GetDevice()->CreateTexture2D(&frameBufferTextureDesc, nullptr, pFrameBuffer.GetAddressOf()));

    // create render target view of frame buffer texture
    D3D11_RENDER_TARGET_VIEW_DESC descRTV = {};
    descRTV.Format = frameBufferTextureDesc.Format;
    descRTV.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
    D3D_THROW_INFO_EXCEPTION(renderer->GetDevice()->CreateRenderTargetView(pFrameBuffer.Get(), &descRTV, m_pRenderTargetView.GetAddressOf()));

    // create shader resource view of frame buffer texture
    D3D11_SHADER_RESOURCE_VIEW_DESC descSRV = {};
    descSRV.Format = frameBufferTextureDesc.Format;
    descSRV.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    descSRV.Texture2D.MostDetailedMip = 0;
    descSRV.Texture2D.MipLevels = 1;
    D3D_THROW_INFO_EXCEPTION(renderer->GetDevice()->CreateShaderResourceView(pFrameBuffer.Get(), &descSRV, m_pShaderResourceView.GetAddressOf()));

    // create depth stencil texture
    Microsoft::WRL::ComPtr<ID3D11Texture2D> pDepthStencil;
    D3D11_TEXTURE2D_DESC descDepth = {};
    descDepth.Width = m_width;
    descDepth.Height = m_height;
    descDepth.MipLevels = 1u;
    descDepth.ArraySize = 1u;
    descDepth.Format = DXGI_FORMAT_D32_FLOAT;
    descDepth.SampleDesc.Count = 1u;
    descDepth.SampleDesc.Quality = 0u;
    descDepth.Usage = D3D11_USAGE_DEFAULT;
    descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    D3D_THROW_INFO_EXCEPTION(renderer->GetDevice()->CreateTexture2D(&descDepth, nullptr, pDepthStencil.GetAddressOf()));

    // create view of depth stencil texture
    D3D11_DEPTH_STENCIL_VIEW_DESC descDSV = {};
    descDSV.Format = DXGI_FORMAT_D32_FLOAT;
    descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    descDSV.Texture2D.MipSlice = 0u;
    D3D_THROW_INFO_EXCEPTION(renderer->GetDevice()->CreateDepthStencilView(pDepthStencil.Get(), &descDSV, m_pDepthStencilView.GetAddressOf()));

}

}  // end namespace SD::RENDER
