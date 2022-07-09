#include "index_buffer.hpp"

#include "exceptions.hpp"


IndexBuffer::IndexBuffer(Renderer* renderer, const std::vector<unsigned short>& indices)
	: m_count(static_cast<UINT>(indices.size()))
{
	D3D_DEBUG_LAYER(renderer);

	D3D11_BUFFER_DESC indexBufferDesc = {};
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.CPUAccessFlags = 0u;
	indexBufferDesc.MiscFlags = 0u;
	indexBufferDesc.ByteWidth = UINT(sizeof(unsigned short) * m_count);
	indexBufferDesc.StructureByteStride = sizeof(unsigned short);

	D3D11_SUBRESOURCE_DATA indexBufferData = {};
	indexBufferData.pSysMem = indices.data();

	D3D_THROW_INFO_EXCEPTION(renderer->GetDevice()->CreateBuffer(&indexBufferDesc, &indexBufferData, m_pIndexBuffer.GetAddressOf()));
}

void IndexBuffer::Bind(Renderer* renderer) const
{
	D3D_DEBUG_LAYER(renderer);

	D3D_THROW_IF_INFO(renderer->GetContext()->IASetIndexBuffer(m_pIndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0u));
}

UINT IndexBuffer::GetIndicesCount() const
{
	return m_count;
}
