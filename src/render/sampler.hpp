#pragma once

#include <d3d11.h>
#include <wrl/client.h>


namespace SD::RENDER {

	class Renderer;

class Sampler
{
public:
	Sampler(Renderer* renderer, bool wrap = true);

	void Bind(Renderer* renderer, UINT slot);

private:
	Microsoft::WRL::ComPtr<ID3D11SamplerState> m_pSampler;
};

}  // end namespace SD::RENDER
