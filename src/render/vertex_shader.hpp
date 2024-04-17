#pragma once

#include <d3d11.h>
#include <wrl/client.h>

#include <string>


namespace SD::RENDER {

const std::wstring VS_PATH = L"src\\shaders\\";

class Renderer;

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
