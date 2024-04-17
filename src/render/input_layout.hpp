#pragma once

#include <d3d11.h>
#include <wrl/client.h>

#include <vector>


namespace SD::RENDER {

	class Renderer;

class InputLayout
{
public:
	InputLayout(Renderer* renderer, const std::vector<D3D11_INPUT_ELEMENT_DESC>& layout, ID3DBlob* pVSBytecode);

	void Bind(Renderer* renderer);

private:
	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_pInputLayout;
};

}  // end namespace SD::RENDER
