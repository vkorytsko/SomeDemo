#include "pixel_shader.hpp"

#include <d3dcompiler.h>

#include "renderer.hpp"
#include "debug_layer.hpp"
#include <exceptions.hpp>


namespace SD::RENDER {

PixelShader::PixelShader(Renderer* renderer, const std::wstring& name)
{
	D3D_DEBUG_LAYER(renderer);

	D3D_THROW_INFO_EXCEPTION(D3DReadFileToBlob((PS_PATH + name).c_str(), m_pBytecodeBlob.GetAddressOf()));
	D3D_THROW_INFO_EXCEPTION(renderer->GetDevice()->CreatePixelShader(
		m_pBytecodeBlob->GetBufferPointer(), m_pBytecodeBlob->GetBufferSize(), nullptr, m_pPixelShader.GetAddressOf())
	);
}

void PixelShader::Bind(Renderer* renderer)
{
	D3D_DEBUG_LAYER(renderer);

	D3D_THROW_IF_INFO(renderer->GetContext()->PSSetShader(m_pPixelShader.Get(), nullptr, 0u));
}

ID3DBlob* PixelShader::GetBytecode() const
{
	return m_pBytecodeBlob.Get();
}

}  // end namespace SD::RENDER
