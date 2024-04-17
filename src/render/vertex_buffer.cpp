#include "vertex_buffer.hpp"

#include "renderer.hpp"
#include "debug_layer.hpp"
#include <exceptions.hpp>


namespace SD::RENDER {

void VertexBuffer::Bind(Renderer* renderer, UINT slot, UINT stride, UINT offset) const
{
	D3D_DEBUG_LAYER(renderer);

	D3D_THROW_IF_INFO(renderer->GetContext()->IASetVertexBuffers(slot, 1u, m_pBuffer.GetAddressOf(), &stride, &offset));
}

D3D11_BUFFER_DESC VertexBuffer::getDescriptor(const size_t byteLength) const
{
	D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.CPUAccessFlags = 0u;
	bufferDesc.MiscFlags = 0u;
	bufferDesc.ByteWidth = static_cast<UINT>(byteLength);

	return bufferDesc;
}

}  // end namespace SD::RENDER
