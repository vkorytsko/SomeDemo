#pragma once

#include <wrl.h>
#include <d3d11.h>
#include <DirectXMath.h>

#include <memory>

#include "debug_layer.hpp"
#include "timer.hpp"

#include "index_buffer.hpp"
#include "input_layout.hpp"
#include "pixel_shader.hpp"
#include "sampler.hpp"
#include "texture.hpp"
#include "vertex_buffer.hpp"
#include "vertex_shader.hpp"

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
        alignas(16) float shiness;
    };

    struct CB_posLight
    {
        alignas(16) dx::XMFLOAT3 position;
        alignas(16) dx::XMFLOAT3 ambient;
        alignas(16) dx::XMFLOAT3 diffuse;
        alignas(16) dx::XMFLOAT3 specular;
        alignas(16) dx::XMFLOAT3 attenuation;  // {constant, linear, quadratic}
    };

    struct CB_dirLight
    {
        alignas(16) dx::XMFLOAT3 direction;
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
    std::unique_ptr<VertexBuffer> m_pBoxVertexBuffer;
    std::unique_ptr<IndexBuffer> m_pBoxIndexBuffer;
    std::unique_ptr<VertexShader> m_pBoxVertexShader;
    std::unique_ptr<PixelShader> m_pBoxPixelShader;
    std::unique_ptr<InputLayout> m_pBoxInputLayout;
    std::unique_ptr<Texture> m_pBoxDiffuseTexture;
    std::unique_ptr<Texture> m_pBoxSpecularTexture;
    std::unique_ptr<Sampler> m_pBoxSampler;
    dx::XMFLOAT3 m_boxPosition = { 0.0f, 0.0f, 0.0f };
    dx::XMFLOAT3 m_boxRotation = { 0.0f, 0.0f, 0.0f };
    dx::XMFLOAT3 m_boxScale = { 1.0f, 1.0f, 1.0f };

    // Light
    std::unique_ptr<VertexBuffer> m_pLightVertexBuffer;
    std::unique_ptr<IndexBuffer> m_pLightIndexBuffer;
    std::unique_ptr<VertexShader> m_pLightVertexShader;
    std::unique_ptr<PixelShader> m_pLightPixelShader;
    std::unique_ptr<InputLayout> m_pLightInputLayout;
    dx::XMFLOAT3 m_lightPosition = { 0.0f, 2.0f, 0.0f };
    dx::XMFLOAT3 m_lightRotation = { 0.0f, 0.0f, 0.0f };
    dx::XMFLOAT3 m_lightScale = { 0.2f, 0.2f, 0.2f };
    dx::XMFLOAT3 m_lightColor = { 1.0f, 1.0f, 1.0f };

    // Grass
    std::unique_ptr<VertexBuffer> m_pGrassVertexBuffer;
    std::unique_ptr<IndexBuffer> m_pGrassIndexBuffer;
    std::unique_ptr<VertexShader> m_pGrassVertexShader;
    std::unique_ptr<PixelShader> m_pGrassPixelShader;
    std::unique_ptr<InputLayout> m_pGrassInputLayout;
    std::unique_ptr<Texture> m_pGrassTexture;
    std::unique_ptr<Sampler> m_pGrassSampler;
    dx::XMFLOAT3 m_grassPosition = { 0.0f, 0.0f, -1.0f };
    dx::XMFLOAT3 m_grassRotation = { 0.0f, 0.0f, 0.0f };
    dx::XMFLOAT3 m_grassScale = { 1.0f, 1.0f, 1.0f };
    wrl::ComPtr<ID3D11BlendState> m_pBlendStateEnabled;
    wrl::ComPtr<ID3D11BlendState> m_pBlendStateDisabled;
    wrl::ComPtr<ID3D11RasterizerState> m_pRasterizerNoCull;
    wrl::ComPtr<ID3D11RasterizerState> m_pRasterizerCull;

    // Floor
    std::unique_ptr<VertexBuffer> m_pFloorVertexBuffer;
    std::unique_ptr<IndexBuffer> m_pFloorIndexBuffer;
    std::unique_ptr<VertexShader> m_pFloorVertexShader;
    std::unique_ptr<PixelShader> m_pFloorPixelShader;
    std::unique_ptr<InputLayout> m_pFloorInputLayout;
    std::unique_ptr<Texture> m_pFloorTexture;
    std::unique_ptr<Sampler> m_pFloorSampler;
    dx::XMFLOAT3 m_floorPosition = { 0.0f, -0.5f, 0.0f };
    dx::XMFLOAT3 m_floorRotation = { 0.0f, 0.0f, 0.0f };
    dx::XMFLOAT3 m_floorScale = { 1.0f, 1.0f, 1.0f };
};
