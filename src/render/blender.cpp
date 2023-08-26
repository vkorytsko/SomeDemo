#include "blender.hpp"

#include "exceptions.hpp"


namespace SD::RENDER {

Blender::Blender(Renderer* renderer, bool enabled)
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
        blendDescRT.SrcBlendAlpha = D3D11_BLEND_SRC_ALPHA;
        blendDescRT.DestBlendAlpha = D3D11_BLEND_DEST_ALPHA;
        blendDescRT.BlendOpAlpha = D3D11_BLEND_OP_ADD;
        blendDescRT.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
    }
    else
    {
        blendDescRT.BlendEnable = FALSE;
        blendDescRT.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
    }

    D3D_THROW_IF_INFO(renderer->GetDevice()->CreateBlendState(&blendDesc, m_pBlender.GetAddressOf()));
}

void Blender::Bind(Renderer* renderer)
{
    D3D_DEBUG_LAYER(renderer);

    D3D_THROW_IF_INFO(renderer->GetContext()->OMSetBlendState(m_pBlender.Get(), nullptr, 0xFFFFFFFFu));
}

}  // end namespace SD::RENDER
