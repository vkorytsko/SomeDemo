#pragma once

#include <d3d11.h>
#include <wrl/client.h>

#include <vector>

#include "renderer.hpp"
#include "exceptions.hpp"


class IndexBuffer
{
public:
	IndexBuffer(Renderer* renderer, const std::vector<unsigned short>& indices);

	void Bind(Renderer* renderer) const;

private:
	UINT m_count;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_pIndexBuffer;
};
