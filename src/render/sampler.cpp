#include "sampler.hpp"

#include "exceptions.hpp"


Sampler::Sampler(Renderer* renderer)
{
    D3D_DEBUG_LAYER(renderer);

    D3D11_SAMPLER_DESC samplerDesc = {};
    samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    D3D_THROW_INFO_EXCEPTION(renderer->GetDevice()->CreateSamplerState(&samplerDesc, m_pSampler.GetAddressOf()));
}

void Sampler::Bind(Renderer* renderer)
{
    D3D_DEBUG_LAYER(renderer);

    D3D_THROW_IF_INFO(renderer->GetContext()->PSSetSamplers(0, 1, m_pSampler.GetAddressOf()));
}
