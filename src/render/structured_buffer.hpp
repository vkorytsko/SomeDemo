#pragma once

#include <d3d11.h>
#include <wrl/client.h>

#include "renderer.hpp"
#include "debug_layer.hpp"
#include <exceptions.hpp>


namespace SD::RENDER {

template<class C>
class StructuredBuffer
{
public:
	StructuredBuffer(Renderer* renderer, std::vector<C>& data)
		: m_data(std::move(data))
	{
		D3D_DEBUG_LAYER(renderer);

		D3D11_BUFFER_DESC structuredBufferDesc;
		structuredBufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		structuredBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		structuredBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		structuredBufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
		structuredBufferDesc.ByteWidth = static_cast<UINT>(sizeof(C) * m_data.capacity());
		structuredBufferDesc.StructureByteStride = sizeof(C);

		if (!m_data.empty())
		{
			D3D11_SUBRESOURCE_DATA structuredBufferData = {};
			structuredBufferData.pSysMem = static_cast<void*>(m_data.data());
			D3D_THROW_INFO_EXCEPTION(renderer->GetDevice()->CreateBuffer(&structuredBufferDesc, &structuredBufferData, &m_pStructuredBuffer));
		}
		else
		{
			D3D_THROW_INFO_EXCEPTION(renderer->GetDevice()->CreateBuffer(&structuredBufferDesc, nullptr, &m_pStructuredBuffer));
		}

		D3D11_SHADER_RESOURCE_VIEW_DESC bufferSRVDesc = {};
		bufferSRVDesc.Format = DXGI_FORMAT_UNKNOWN;
		bufferSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
		bufferSRVDesc.Buffer.FirstElement = 0;
		bufferSRVDesc.Buffer.NumElements = static_cast<UINT>(m_data.capacity());
		D3D_THROW_INFO_EXCEPTION(renderer->GetDevice()->CreateShaderResourceView(m_pStructuredBuffer.Get(), &bufferSRVDesc, m_pBufferSRV.GetAddressOf()));
	}

	void Update(Renderer* renderer)
	{
		D3D_DEBUG_LAYER(renderer);

		D3D11_MAPPED_SUBRESOURCE mappedData;
		D3D_THROW_IF_INFO(renderer->GetContext()->Map(m_pStructuredBuffer.Get(), 0u, D3D11_MAP_WRITE_DISCARD, 0u, &mappedData));
		memcpy(mappedData.pData, m_data.data(), sizeof(C) * m_data.capacity());
		D3D_THROW_IF_INFO(renderer->GetContext()->Unmap(m_pStructuredBuffer.Get(), 0u));
	}

	void VSBind(Renderer* renderer, UINT slot) const
	{
		D3D_DEBUG_LAYER(renderer);

		D3D_THROW_IF_INFO(renderer->GetContext()->VSSetShaderResources(slot, 1u, m_pBufferSRV.GetAddressOf()));
	}

	void PSBind(Renderer* renderer, UINT slot) const
	{
		D3D_DEBUG_LAYER(renderer);

		D3D_THROW_IF_INFO(renderer->GetContext()->PSSetShaderResources(slot, 1u, m_pBufferSRV.GetAddressOf()));
	}

	std::vector<C>& GetData()
	{
		return m_data;
	}

private:
	std::vector<C> m_data;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_pStructuredBuffer;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_pBufferSRV;
};

}  // end namespace SD::RENDER
