#pragma once

#include <d3d11.h>
#include <wrl/client.h>

#include <vector>

#include "renderer.hpp"
#include "exceptions.hpp"


namespace SD::RENDER {

template<class V>
class VertexBuffer
{
public:
	template<class V>
	VertexBuffer(Renderer* renderer, const std::vector<V>& vertices)
		: m_stride(sizeof(V))
	{
		D3D_DEBUG_LAYER(renderer);

		D3D11_BUFFER_DESC vertexBufferDesc = {};
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		vertexBufferDesc.CPUAccessFlags = 0u;
		vertexBufferDesc.MiscFlags = 0u;
		vertexBufferDesc.ByteWidth = UINT(sizeof(V) * vertices.size());
		vertexBufferDesc.StructureByteStride = sizeof(V);

		D3D11_SUBRESOURCE_DATA vertexBufferData = {};
		vertexBufferData.pSysMem = vertices.data();

		D3D_THROW_INFO_EXCEPTION(renderer->GetDevice()->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &m_pVertexBuffer));
	}

	void Bind(Renderer* renderer) const
	{
		D3D_DEBUG_LAYER(renderer);

		const UINT offset = 0u;
		D3D_THROW_IF_INFO(renderer->GetContext()->IASetVertexBuffers(0u, 1u, m_pVertexBuffer.GetAddressOf(), &m_stride, &offset));
	}

private:
	UINT m_stride;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_pVertexBuffer;
};

}  // end namespace SD::RENDER
