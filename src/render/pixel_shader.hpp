#pragma once

#include <d3d11.h>
#include <wrl.h>

#include <string>


namespace SD::RENDER {

const std::wstring PS_PATH = L"src\\shaders\\";

class Renderer;

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

}  // end namespace SD::RENDER
