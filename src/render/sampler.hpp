#pragma once

#include <d3d11.h>
#include <wrl/client.h>

#include "renderer.hpp"


namespace SD::RENDER {

class Sampler
{
public:
	Sampler(Renderer* renderer);

	void Bind(Renderer* renderer);

private:
	Microsoft::WRL::ComPtr<ID3D11SamplerState> m_pSampler;
};

}  // end namespace SD::RENDER
