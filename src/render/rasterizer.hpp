#pragma once

#include <d3d11.h>
#include <wrl/client.h>


namespace SD::RENDER {

	class Renderer;

class Rasterizer
{
public:
	Rasterizer(Renderer* renderer, bool cull);

	void Bind(Renderer* renderer);

private:
	bool m_cull = true;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_pRasterizer;
};

}  // end namespace SD::RENDER
