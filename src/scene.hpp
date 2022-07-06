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

    struct Vertex2
    {
        dx::XMFLOAT3 pos;
    };

    struct Vertex3
    {
        dx::XMFLOAT3 pos;
        dx::XMFLOAT3 normal;
        dx::XMFLOAT2 uv;
    };

    struct CB_transform
    {
        dx::XMMATRIX model;
        dx::XMMATRIX view;
        dx::XMMATRIX projection;
        alignas(16) dx::XMFLOAT3 viewPosition;
    };

    struct CB_material
    {
        alignas(16) dx::XMFLOAT3 ambient;
        alignas(16) dx::XMFLOAT3 diffuse;
        alignas(16) dx::XMFLOAT3 specular;
        float shiness;
    };

    struct CB_light
    {
        alignas(16) dx::XMFLOAT3 position;
        alignas(16) dx::XMFLOAT3 ambient;
        alignas(16) dx::XMFLOAT3 diffuse;
        alignas(16) dx::XMFLOAT3 specular;
    };

public:
	Scene();
	~Scene();

	void Setup();
	void Update(float dt);
	void Draw();

private:
    void SetupBox();
    void SetupLight();
    void SetupGrass();
    void SetupFloor();

    void UpdateBox(float dt);
    void UpdateLight(float dt);
    void UpdateGrass(float dt);
    void UpdateFloor(float dt);

    void DrawBox();
    void DrawLight();
    void DrawGrass();
    void DrawFloor();

private:
    std::unique_ptr<Timer> m_pTimer;

    // Box
    wrl::ComPtr<ID3D11Buffer> m_pBoxVertexBuffer;
    wrl::ComPtr<ID3D11Buffer> m_pBoxIndexBuffer;
    wrl::ComPtr<ID3D11VertexShader> m_pBoxVertexShader;
    wrl::ComPtr<ID3D11PixelShader> m_pBoxPixelShader;
    wrl::ComPtr<ID3D11InputLayout> m_pBoxInputLayout;
    wrl::ComPtr<ID3D11ShaderResourceView> m_pBoxTextureView;
    wrl::ComPtr<ID3D11SamplerState> m_pBoxSampler;
    dx::XMFLOAT3 m_boxPosition = { 0.0f, 0.0f, 0.0f };
    dx::XMFLOAT3 m_boxRotation = { 0.0f, 0.0f, 0.0f };
    dx::XMFLOAT3 m_boxScale = { 1.0f, 1.0f, 1.0f };

    // Light
    wrl::ComPtr<ID3D11Buffer> m_pLightVertexBuffer;
    wrl::ComPtr<ID3D11Buffer> m_pLightIndexBuffer;
    wrl::ComPtr<ID3D11VertexShader> m_pLightVertexShader;
    wrl::ComPtr<ID3D11PixelShader> m_pLightPixelShader;
    wrl::ComPtr<ID3D11InputLayout> m_pLightInputLayout;
    dx::XMFLOAT3 m_lightPosition = { 0.0f, 2.0f, 0.0f };
    dx::XMFLOAT3 m_lightRotation = { 0.0f, 0.0f, 0.0f };
    dx::XMFLOAT3 m_lightScale = { 0.2f, 0.2f, 0.2f };
    dx::XMFLOAT3 m_lightColor = { 1.0f, 1.0f, 1.0f };

    // Grass
    wrl::ComPtr<ID3D11Buffer> m_pGrassVertexBuffer;
    wrl::ComPtr<ID3D11Buffer> m_pGrassIndexBuffer;
    wrl::ComPtr<ID3D11VertexShader> m_pGrassVertexShader;
    wrl::ComPtr<ID3D11PixelShader> m_pGrassPixelShader;
    wrl::ComPtr<ID3D11InputLayout> m_pGrassInputLayout;
    wrl::ComPtr<ID3D11ShaderResourceView> m_pGrassTextureView;
    wrl::ComPtr<ID3D11SamplerState> m_pGrassSampler;
    dx::XMFLOAT3 m_grassPosition = { 0.0f, 0.0f, -1.0f };
    dx::XMFLOAT3 m_grassRotation = { 0.0f, 0.0f, 0.0f };
    dx::XMFLOAT3 m_grassScale = { 1.0f, 1.0f, 1.0f };
    wrl::ComPtr<ID3D11BlendState> m_pBlendStateEnabled;
    wrl::ComPtr<ID3D11BlendState> m_pBlendStateDisabled;
    wrl::ComPtr<ID3D11RasterizerState> m_pRasterizerNoCull;
    wrl::ComPtr<ID3D11RasterizerState> m_pRasterizerCull;

    // Floor
    wrl::ComPtr<ID3D11Buffer> m_pFloorVertexBuffer;
    wrl::ComPtr<ID3D11Buffer> m_pFloorIndexBuffer;
    wrl::ComPtr<ID3D11VertexShader> m_pFloorVertexShader;
    wrl::ComPtr<ID3D11PixelShader> m_pFloorPixelShader;
    wrl::ComPtr<ID3D11InputLayout> m_pFloorInputLayout;
    wrl::ComPtr<ID3D11ShaderResourceView> m_pFloorTextureView;
    wrl::ComPtr<ID3D11SamplerState> m_pFloorSampler;
    dx::XMFLOAT3 m_floorPosition = { 0.0f, -0.5f, 0.0f };
    dx::XMFLOAT3 m_floorRotation = { 0.0f, 0.0f, 0.0f };
    dx::XMFLOAT3 m_floorScale = { 1.0f, 1.0f, 1.0f };

#ifndef NDEBUG
    DebugLayer m_debugLayer;
#endif
};
