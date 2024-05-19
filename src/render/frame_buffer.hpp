#pragma once

#include <cstdint>
#include <d3d11.h>
#include <wrl/client.h>


namespace SD::RENDER {

class Renderer;

class FrameBuffer
{
public:
	FrameBuffer(Renderer* renderer, const float width, const float height);
	~FrameBuffer() = default;

	void bind(Renderer* renderer, bool depth = true) const;

	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> getRTV() const { return m_pRenderTargetView; }
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> getSRV() const { return m_pShaderResourceView; }
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> getDSV() const { return m_pDepthStencilView; }

	void resize(Renderer* renderer, const UINT width, const UINT height);

	UINT width() const { return m_width; }
	UINT height() const { return m_height; }

private:
	void createTextures(const Renderer* renderer);
	void createViews(const Renderer* renderer);
	void createStates(const Renderer* renderer);

private:
	UINT m_width;
	UINT m_height;

	Microsoft::WRL::ComPtr<ID3D11Texture2D> m_pRenderTarget;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> m_pDepthStencil;

	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_pRenderTargetView;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_pShaderResourceView;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_pDepthStencilView;

	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> m_pDepthStencilStateEnabled;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> m_pDepthStencilStateDisabled;
};


class CubeFrameBuffer
{
private:
	static constexpr uint8_t FACE_COUNT = 6;
public:
	CubeFrameBuffer(Renderer* renderer, const float size);
	~CubeFrameBuffer() = default;

	void bind(Renderer* renderer) const;

	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> getRTV(uint8_t face) const { return m_pRenderTargetView[face]; }
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> getSRV() const { return m_pShaderResourceView; }
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> getDSV(uint8_t face) const { return m_pDepthStencilView[face]; }

	void resize(Renderer* renderer, const UINT size);

	UINT size() const { return m_size; }

private:
	void createTextures(const Renderer* renderer);
	void createViews(const Renderer* renderer);
	void createStates(const Renderer* renderer);

private:
	UINT m_size;

	Microsoft::WRL::ComPtr<ID3D11Texture2D> m_pRenderTarget;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> m_pDepthStencil;

	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_pRenderTargetView[6];
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_pShaderResourceView;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_pDepthStencilView[6];

	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> m_pDepthStencilState;
};

}  // end namespace SD::RENDER
