#include "blend_state.hpp"

#include "exceptions.hpp"


BlendState::BlendState(Renderer* renderer, bool enabled)
    : m_enabled(enabled)
{
    D3D_DEBUG_LAYER(renderer);

    D3D11_BLEND_DESC blendDesc = {};
    auto& blendDescRT = blendDesc.RenderTarget[0];
    if (m_enabled)
    {
        blendDescRT.BlendEnable = TRUE;
        blendDescRT.SrcBlend = D3D11_BLEND_SRC_ALPHA;
        blendDescRT.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
        blendDescRT.BlendOp = D3D11_BLEND_OP_ADD;
        blendDescRT.SrcBlendAlpha = D3D11_BLEND_ZERO;
        blendDescRT.DestBlendAlpha = D3D11_BLEND_ZERO;
        blendDescRT.BlendOpAlpha = D3D11_BLEND_OP_ADD;
        blendDescRT.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
    }
    else
    {
        blendDescRT.BlendEnable = FALSE;
        blendDescRT.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
    }

    D3D_THROW_IF_INFO(renderer->GetDevice()->CreateBlendState(&blendDesc, m_pBlendState.GetAddressOf()));
}

void BlendState::Bind(Renderer* renderer)
{
    D3D_DEBUG_LAYER(renderer);

    D3D_THROW_IF_INFO(renderer->GetContext()->OMSetBlendState(m_pBlendState.Get(), nullptr, 0xFFFFFFFFu));
}
