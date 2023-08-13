#pragma once

#include <d3d11.h>
#include <DirectXTex.h>
#include <wrl/client.h>

#include <string>

#include "renderer.hpp"


namespace SD::RENDER {


class Texture
{
public:
	Texture(Renderer* renderer, const std::wstring& path);
	void Bind(Renderer* renderer, UINT slot) const;

private:
	DirectX::ScratchImage Load(const std::wstring& path);

private:
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_pTextureView;
};

}  // end namespace SD::RENDER
