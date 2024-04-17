#pragma once

#include <d3d11.h>
#include <wrl/client.h>


namespace SD::RENDER {

	class Renderer;

class Sampler
{
public:
	Sampler(Renderer* renderer);

	void Bind(Renderer* renderer);

private:
	Microsoft::WRL::ComPtr<ID3D11SamplerState> m_pSampler;
};

}  // end namespace SD::RENDER
