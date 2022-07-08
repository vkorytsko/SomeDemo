#pragma once

#include <string>
#include <DirectXTex.h>

#include "renderer.hpp"


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