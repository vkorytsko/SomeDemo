#pragma once

#include <d3d11.h>
#include <wrl/client.h>

#include "renderer.hpp"


namespace SD::RENDER {

class Blender
{
public:
	Blender(Renderer* renderer, bool enabled);

	void Bind(Renderer* renderer);

private:
	bool m_enabled = false;
	Microsoft::WRL::ComPtr<ID3D11BlendState> m_pBlender;
};

}  // end namespace SD::RENDER
