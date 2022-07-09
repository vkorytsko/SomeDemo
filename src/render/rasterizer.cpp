#include "rasterizer.hpp"

#include "exceptions.hpp"


Rasterizer::Rasterizer(Renderer* renderer, bool cull)
    : m_cull(cull)
{
    D3D_DEBUG_LAYER(renderer);

    D3D11_RASTERIZER_DESC rasterizerDesc = CD3D11_RASTERIZER_DESC(CD3D11_DEFAULT{});
    rasterizerDesc.CullMode = cull ? D3D11_CULL_BACK : D3D11_CULL_NONE;

    D3D_THROW_INFO_EXCEPTION(renderer->GetDevice()->CreateRasterizerState(&rasterizerDesc, m_pRasterizer.GetAddressOf()));
}

void Rasterizer::Bind(Renderer* renderer)
{
    D3D_DEBUG_LAYER(renderer);

    D3D_THROW_IF_INFO(renderer->GetContext()->RSSetState(m_pRasterizer.Get()));
}
