#include "index_buffer.hpp"

#include "exceptions.hpp"


namespace SD::RENDER {

void IndexBuffer::Bind(Renderer* renderer, UINT slot, UINT stride, UINT offset) const
{
	D3D_DEBUG_LAYER(renderer);

	D3D_THROW_IF_INFO(renderer->GetContext()->IASetIndexBuffer(m_pBuffer.Get(), DXGI_FORMAT_R16_UINT, offset));
}

D3D11_BUFFER_DESC IndexBuffer::getDescriptor(const size_t byteLength) const
{
	D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.CPUAccessFlags = 0u;
	bufferDesc.MiscFlags = 0u;
	bufferDesc.ByteWidth = static_cast<UINT>(byteLength);

	return bufferDesc;
}

}  // end namespace SD::RENDER
