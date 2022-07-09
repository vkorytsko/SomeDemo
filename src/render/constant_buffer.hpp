#pragma once

#include <d3d11.h>
#include <wrl/client.h>

#include "renderer.hpp"
#include "exceptions.hpp"


namespace SD::RENDER {

template<class C>
class ConstantBuffer
{
public:
	ConstantBuffer(Renderer* renderer, C data)
		: m_data(data)
	{
		D3D_DEBUG_LAYER(renderer);

		D3D11_BUFFER_DESC constantBufferDesc;
		constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		constantBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		constantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		constantBufferDesc.MiscFlags = 0u;
		constantBufferDesc.ByteWidth = sizeof(m_data);
		constantBufferDesc.StructureByteStride = 0u;

		D3D11_SUBRESOURCE_DATA constantBufferData = {};
		constantBufferData.pSysMem = &m_data;
		D3D_THROW_INFO_EXCEPTION(renderer->GetDevice()->CreateBuffer(&constantBufferDesc, &constantBufferData, &m_pConstantBuffer));
	}

	void Update(Renderer* renderer)
	{
		D3D_DEBUG_LAYER(renderer);

		D3D11_MAPPED_SUBRESOURCE mappedData;
		D3D_THROW_IF_INFO(renderer->GetContext()->Map(m_pConstantBuffer.Get(), 0u, D3D11_MAP_WRITE_DISCARD, 0u, &mappedData));
		memcpy(mappedData.pData, &m_data, sizeof(m_data));
		D3D_THROW_IF_INFO(renderer->GetContext()->Unmap(m_pConstantBuffer.Get(), 0u));
	}

	void VSBind(Renderer* renderer, UINT slot) const
	{
		D3D_DEBUG_LAYER(renderer);

		D3D_THROW_IF_INFO(renderer->GetContext()->VSSetConstantBuffers(slot, 1u, m_pConstantBuffer.GetAddressOf()));
	}

	void PSBind(Renderer* renderer, UINT slot) const
	{
		D3D_DEBUG_LAYER(renderer);

		D3D_THROW_IF_INFO(renderer->GetContext()->PSSetConstantBuffers(slot, 1u, m_pConstantBuffer.GetAddressOf()));
	}

	C* GetData()
	{
		return &m_data;
	}

private:
	C m_data;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_pConstantBuffer;
};

}  // end namespace SD::RENDER
