#pragma once

#include <d3d11.h>
#include <wrl/client.h>

#include <string>

#include "renderer.hpp"


namespace SD::RENDER {

#ifndef NDEBUG 
const std::wstring VS_PATH = L"Debug\\";
#else
const std::wstring VS_PATH = L"Release\\";
#endif


class VertexShader
{
public:
	VertexShader(Renderer* renderer, const std::wstring& name);

	void Bind(Renderer* renderer);
	ID3DBlob* GetBytecode() const;

private:
	Microsoft::WRL::ComPtr<ID3DBlob> m_pBytecodeBlob;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> m_pVertexShader;
};

}  // end namespace SD::RENDER
