#pragma once

#include <d3d11.h>
#include <wrl/client.h>

#include "renderer.hpp"


class BlendState
{
public:
	BlendState(Renderer* renderer, bool enabled);

	void Bind(Renderer* renderer);

private:
	bool m_enabled = false;
	Microsoft::WRL::ComPtr<ID3D11BlendState> m_pBlendState;
};
