#include "input_layout.hpp"

#include "renderer.hpp"
#include "debug_layer.hpp"
#include <exceptions.hpp>


namespace SD::RENDER {

InputLayout::InputLayout(Renderer* renderer, const std::vector<D3D11_INPUT_ELEMENT_DESC>& layout, ID3DBlob* pVSBytecode)
{
	D3D_DEBUG_LAYER(renderer);

	D3D_THROW_INFO_EXCEPTION(renderer->GetDevice()->CreateInputLayout(
		layout.data(), static_cast<UINT>(layout.size()),
		pVSBytecode->GetBufferPointer(), pVSBytecode->GetBufferSize(),
		m_pInputLayout.GetAddressOf()
	));
}

void InputLayout::Bind(Renderer* renderer)
{
	D3D_DEBUG_LAYER(renderer);

	D3D_THROW_IF_INFO(renderer->GetContext()->IASetInputLayout(m_pInputLayout.Get()));
}

}  // end namespace SD::RENDER
