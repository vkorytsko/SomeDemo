#include "sampler.hpp"

#include "renderer.hpp"
#include "debug_layer.hpp"
#include <exceptions.hpp>


namespace SD::RENDER {

Sampler::Sampler(Renderer* renderer, bool wrap)
{
    D3D_DEBUG_LAYER(renderer);

    D3D11_SAMPLER_DESC samplerDesc = {};
    samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
    samplerDesc.AddressU = wrap ? D3D11_TEXTURE_ADDRESS_WRAP : D3D11_TEXTURE_ADDRESS_CLAMP;
    samplerDesc.AddressV = wrap ? D3D11_TEXTURE_ADDRESS_WRAP : D3D11_TEXTURE_ADDRESS_CLAMP;
    samplerDesc.AddressW = wrap ? D3D11_TEXTURE_ADDRESS_WRAP : D3D11_TEXTURE_ADDRESS_CLAMP;
    samplerDesc.MaxAnisotropy = D3D11_REQ_MAXANISOTROPY;
    samplerDesc.MipLODBias = 0.0f;
    samplerDesc.MinLOD = 0.0f;
    samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
    D3D_THROW_INFO_EXCEPTION(renderer->GetDevice()->CreateSamplerState(&samplerDesc, m_pSampler.GetAddressOf()));
}

void Sampler::Bind(Renderer* renderer, UINT slot)
{
    D3D_DEBUG_LAYER(renderer);

    D3D_THROW_IF_INFO(renderer->GetContext()->PSSetSamplers(slot, 1, m_pSampler.GetAddressOf()));
}

}  // end namespace SD::RENDER
