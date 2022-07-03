#pragma once

#include <wrl.h>
#include <d3d11.h>
#include <DirectXMath.h>

#include <memory>

#include "debug_layer.hpp"
#include "timer.hpp"

namespace wrl = Microsoft::WRL;
namespace dx = DirectX;


class Scene
{
public:
    struct Vertex
    {
        dx::XMFLOAT3 pos;
        dx::XMFLOAT2 uv;
    };

public:
	Scene();
	~Scene();

	void Setup();
	void Update(float dt);
	void Draw();

private:
    std::unique_ptr<Timer> m_pTimer;

    wrl::ComPtr<ID3D11Buffer> m_pVertexBuffer;
    wrl::ComPtr<ID3D11Buffer> m_pIndexBuffer;
    wrl::ComPtr<ID3D11VertexShader> m_pVertexShader;
    wrl::ComPtr<ID3D11PixelShader> m_pPixelShader;
    wrl::ComPtr<ID3D11InputLayout> m_pInputLayout;
    wrl::ComPtr<ID3D11ShaderResourceView> m_pTextureView;
    wrl::ComPtr<ID3D11SamplerState> m_pSampler;

    float m_yaw = 0.0f;
    float m_pitch = 0.0f;

#ifndef NDEBUG
    DebugLayer m_debugLayer;
#endif
};
