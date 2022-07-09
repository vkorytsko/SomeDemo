#pragma once

#include <d3d11.h>
#include <wrl/client.h>

#include <vector>

#include "renderer.hpp"


namespace SD::RENDER {

class IndexBuffer
{
public:
	IndexBuffer(Renderer* renderer, const std::vector<unsigned short>& indices);

	void Bind(Renderer* renderer) const;
	UINT GetIndicesCount() const;

private:
	UINT m_count;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_pIndexBuffer;
};

}  // end namespace SD::RENDER
