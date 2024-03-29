#include "sampler.hpp"

#include "exceptions.hpp"


namespace SD::RENDER {

Sampler::Sampler(Renderer* renderer)
{
    D3D_DEBUG_LAYER(renderer);

    D3D11_SAMPLER_DESC samplerDesc = {};
    samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.MaxAnisotropy = D3D11_REQ_MAXANISOTROPY;
    samplerDesc.MipLODBias = 0.0f;
    samplerDesc.MinLOD = 0.0f;
    samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
    D3D_THROW_INFO_EXCEPTION(renderer->GetDevice()->CreateSamplerState(&samplerDesc, m_pSampler.GetAddressOf()));
}

void Sampler::Bind(Renderer* renderer)
{
    D3D_DEBUG_LAYER(renderer);

    D3D_THROW_IF_INFO(renderer->GetContext()->PSSetSamplers(0, 1, m_pSampler.GetAddressOf()));
}

}  // end namespace SD::RENDER
