#pragma once

#include <d3d11.h>
#include <wrl/client.h>

#include "renderer.hpp"


namespace SD::RENDER {

class Buffer
{
public:
	Buffer() = default;
	virtual ~Buffer() = default;

	void create(Renderer* renderer, const void* data, const size_t byteLength);

	virtual void Bind(Renderer* renderer, UINT slot, UINT stride, UINT offset) const = 0;

protected:
	virtual D3D11_BUFFER_DESC getDescriptor(const size_t byteLength) const = 0;

protected:
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_pBuffer;
};

}  // end namespace SD::RENDER
