#pragma once

#include "buffer.hpp"


namespace SD::RENDER {

class VertexBuffer : public Buffer
{
public:
	VertexBuffer() = default;
	~VertexBuffer() override = default;

	void Bind(Renderer* renderer, UINT slot, UINT stride, UINT offset) const override;

protected:
	D3D11_BUFFER_DESC getDescriptor(const size_t byteLength) const override;
};

}  // end namespace SD::RENDER
