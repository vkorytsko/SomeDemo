#include "vertex_buffer.hpp"


void VertexBuffer::Bind(Renderer* renderer) const
{
	D3D_DEBUG_LAYER(renderer);

	const UINT offset = 0u;
	D3D_THROW_IF_INFO(renderer->GetContext()->IASetVertexBuffers(0u, 1u, m_pVertexBuffer.GetAddressOf(), &m_stride, &offset));
}
