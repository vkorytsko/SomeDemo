#pragma once

#include <d3d11.h>
#include <wrl/client.h>

#include "renderer.hpp"


namespace SD::RENDER {

class FrameBuffer
{
public:
	FrameBuffer(Renderer* renderer, const float width, const float height);
	~FrameBuffer() = default;

	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> getRTV() const;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> getSRV() const;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> getDSV() const;

	void resize(Renderer* renderer, const UINT width, const UINT height);

	UINT width() const { return m_width; }
	UINT height() const { return m_height; }

private:
	void createViews(Renderer* renderer);

private:
	UINT m_width;
	UINT m_height;

	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_pRenderTargetView;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_pShaderResourceView;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_pDepthStencilView;
};

}  // end namespace SD::RENDER
