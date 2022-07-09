#include "vertex_shader.hpp"

#include <d3dcompiler.h>

#include "exceptions.hpp"


VertexShader::VertexShader(Renderer* renderer, const std::wstring& name)
{
	D3D_DEBUG_LAYER(renderer);

	D3D_THROW_INFO_EXCEPTION(D3DReadFileToBlob((VS_PATH + name).c_str(), m_pBytecodeBlob.GetAddressOf()));
	D3D_THROW_INFO_EXCEPTION(renderer->GetDevice()->CreateVertexShader(
		m_pBytecodeBlob->GetBufferPointer(), m_pBytecodeBlob->GetBufferSize(), nullptr, m_pVertexShader.GetAddressOf())
	);
}

void VertexShader::Bind(Renderer* renderer)
{
	D3D_DEBUG_LAYER(renderer);

	D3D_THROW_IF_INFO(renderer->GetContext()->VSSetShader(m_pVertexShader.Get(), nullptr, 0u));
}

ID3DBlob* VertexShader::GetBytecode() const
{
	return m_pBytecodeBlob.Get();
}
