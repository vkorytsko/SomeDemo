#pragma once

#include <d3d11.h>
#include <DirectXTex.h>
#include <wrl/client.h>

#include <string>

#include "renderer.hpp"


namespace SD::RENDER {

const std::wstring TEXTURES_PATH = L"..\\res\\textures\\";


class Texture
{
public:
	Texture(Renderer* renderer, const std::wstring& name);
	void Bind(Renderer* renderer, UINT slot);

private:
	DirectX::ScratchImage Load(const std::wstring& name);

private:
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_pTextureView;
};

}  // end namespace SD::RENDER
