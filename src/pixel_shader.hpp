#pragma once

#include <string>

#include "renderer.hpp"


#ifndef NDEBUG
const std::wstring PS_PATH = L"Debug\\";
#else
const std::wstring PS_PATH = L"Release\\";
#endif


class PixelShader
{
public:
	PixelShader(Renderer* renderer, const std::wstring& name);

	void Bind(Renderer* renderer);
	ID3DBlob* GetBytecode() const;

private:
	Microsoft::WRL::ComPtr<ID3DBlob> m_pBytecodeBlob;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> m_pPixelShader;
};
