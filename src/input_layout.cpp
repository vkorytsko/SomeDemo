#include "input_layout.hpp"

#include "exceptions.hpp"


InputLayout::InputLayout(Renderer* renderer, const std::vector<D3D11_INPUT_ELEMENT_DESC>& layout, ID3DBlob* pVSBytecode)
{
	D3D_DEBUG_LAYER(renderer);

	D3D_THROW_INFO_EXCEPTION(renderer->GetDevice()->CreateInputLayout(
		layout.data(), static_cast<UINT>(layout.size()),
		pVSBytecode->GetBufferPointer(), pVSBytecode->GetBufferSize(),
		&m_pInputLayout
	));
}

void InputLayout::Bind(Renderer* renderer)
{
	D3D_DEBUG_LAYER(renderer);

	D3D_THROW_IF_INFO(renderer->GetContext()->IASetInputLayout(m_pInputLayout.Get()));
}
