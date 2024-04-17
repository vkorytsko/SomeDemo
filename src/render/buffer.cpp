#include "buffer.hpp"

#include "renderer.hpp"
#include <exceptions.hpp>
#include "debug_layer.hpp"


namespace SD::RENDER {

void Buffer::create(Renderer* renderer, const void* data, const size_t byteLength)
{
	D3D_DEBUG_LAYER(renderer);

	const auto bufferDesc = getDescriptor(byteLength);

	D3D11_SUBRESOURCE_DATA bufferData = {};
	bufferData.pSysMem = data;

	D3D_THROW_INFO_EXCEPTION(renderer->GetDevice()->CreateBuffer(&bufferDesc, &bufferData, m_pBuffer.GetAddressOf()));
}

}  // end namespace SD::RENDER
