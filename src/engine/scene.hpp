#pragma once

#include <d3d11.h>
#include <DirectXMath.h>

#include <memory>

#include "debug_layer.hpp"
#include "timer.hpp"

#include "blender.hpp"
#include "constant_buffer.hpp"
#include "index_buffer.hpp"
#include "input_layout.hpp"
#include "pixel_shader.hpp"
#include "rasterizer.hpp"
#include "sampler.hpp"
#include "texture.hpp"
#include "vertex_buffer.hpp"
#include "vertex_shader.hpp"


namespace SD::ENGINE {

class Scene
{
public:
    struct Vertex
    {
        DirectX::XMFLOAT3 pos;
        DirectX::XMFLOAT2 uv;
    };

    struct Vertex2
    {
        DirectX::XMFLOAT3 pos;
    };

    struct Vertex3
    {
        DirectX::XMFLOAT3 pos;
        DirectX::XMFLOAT3 normal;
        DirectX::XMFLOAT2 uv;
    };

    struct CB_transform
    {
        DirectX::XMMATRIX model;
        DirectX::XMMATRIX view;
        DirectX::XMMATRIX projection;
        alignas(16) DirectX::XMFLOAT3 viewPosition;
    };

    struct CB_material
    {
        alignas(16) float shiness;
    };

    struct CB_color
    {
        alignas(16) DirectX::XMFLOAT3 color;
    };

    struct CB_posLight
    {
        alignas(16) DirectX::XMFLOAT3 position;
        alignas(16) DirectX::XMFLOAT3 ambient;
        alignas(16) DirectX::XMFLOAT3 diffuse;
        alignas(16) DirectX::XMFLOAT3 specular;
        alignas(16) DirectX::XMFLOAT3 attenuation;  // {constant, linear, quadratic}
    };

    struct CB_dirLight
    {
        alignas(16) DirectX::XMFLOAT3 direction;
        alignas(16) DirectX::XMFLOAT3 ambient;
        alignas(16) DirectX::XMFLOAT3 diffuse;
        alignas(16) DirectX::XMFLOAT3 specular;
    };

    struct CB_spotLight
    {
        alignas(16) DirectX::XMFLOAT3 position;
        alignas(16) DirectX::XMFLOAT3 direction;
        alignas(16) DirectX::XMFLOAT3 ambient;
        alignas(16) DirectX::XMFLOAT3 diffuse;
        alignas(16) DirectX::XMFLOAT3 specular;
        alignas(16) DirectX::XMFLOAT3 attenuation;  // {constant, linear, quadratic}
        alignas(16) DirectX::XMFLOAT2 cutOff;  // {inner, outer}
        int enabled;
    };

public:
	Scene();
	~Scene();

	void Setup();
	void Simulate(float dt);
	void Update(float dt);
	void Draw();

private:
    void SetupBox();
    void SetupLight();
    void SetupGrass();
    void SetupFloor();

    void SimulateBox(float dt);
    void SimulateLight(float dt);
    void SimulateGrass(float dt);
    void SimulateFloor(float dt);

    void UpdateBox(float dt);
    void UpdateLight(float dt);
    void UpdateGrass(float dt);
    void UpdateFloor(float dt);

    void DrawBox();
    void DrawLight();
    void DrawGrass();
    void DrawFloor();

    DirectX::XMMATRIX GetModelMatrix(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& rotation, const DirectX::XMFLOAT3& scale) const;

private:
    // Common
    std::unique_ptr<Timer> m_pTimer;
    float m_simulationTime = 0.0f;

    // Box
    std::unique_ptr<RENDER::VertexBuffer<Vertex3>> m_pBoxVertexBuffer;
    std::unique_ptr<RENDER::IndexBuffer> m_pBoxIndexBuffer;
    std::unique_ptr<RENDER::VertexShader> m_pBoxVertexShader;
    std::unique_ptr<RENDER::PixelShader> m_pBoxPixelShader;
    std::unique_ptr<RENDER::InputLayout> m_pBoxInputLayout;
    std::unique_ptr<RENDER::Texture> m_pBoxDiffuseTexture;
    std::unique_ptr<RENDER::Texture> m_pBoxSpecularTexture;
    std::unique_ptr<RENDER::Sampler> m_pBoxSampler;

    std::unique_ptr<RENDER::ConstantBuffer<CB_transform>> m_pBoxTransformCB;
    std::unique_ptr<RENDER::ConstantBuffer<CB_material>> m_pBoxMaterialCB;
    std::unique_ptr<RENDER::ConstantBuffer<CB_posLight>> m_pBoxPosLightCB;
    std::unique_ptr<RENDER::ConstantBuffer<CB_dirLight>> m_pBoxDirLightCB;
    std::unique_ptr<RENDER::ConstantBuffer<CB_spotLight>> m_pBoxSpotLightCB;

    DirectX::XMFLOAT3 m_boxPosition = { 0.0f, 0.0f, 0.0f };
    DirectX::XMFLOAT3 m_boxRotation = { 0.0f, 0.0f, 0.0f };
    DirectX::XMFLOAT3 m_boxScale = { 1.0f, 1.0f, 1.0f };

    // Light
    std::unique_ptr<RENDER::VertexBuffer<Vertex2>> m_pLightVertexBuffer;
    std::unique_ptr<RENDER::IndexBuffer> m_pLightIndexBuffer;
    std::unique_ptr<RENDER::VertexShader> m_pLightVertexShader;
    std::unique_ptr<RENDER::PixelShader> m_pLightPixelShader;
    std::unique_ptr<RENDER::InputLayout> m_pLightInputLayout;

    std::unique_ptr<RENDER::ConstantBuffer<CB_transform>> m_pLightTransformCB;
    std::unique_ptr<RENDER::ConstantBuffer<CB_color>> m_pLightColorCB;

    DirectX::XMFLOAT3 m_lightPosition = { 0.0f, 2.0f, 0.0f };
    DirectX::XMFLOAT3 m_lightRotation = { 0.0f, 0.0f, 0.0f };
    DirectX::XMFLOAT3 m_lightScale = { 0.2f, 0.2f, 0.2f };
    DirectX::XMFLOAT3 m_lightColor = { 1.0f, 1.0f, 1.0f };

    // Grass
    std::unique_ptr<RENDER::VertexBuffer<Vertex>> m_pGrassVertexBuffer;
    std::unique_ptr<RENDER::IndexBuffer> m_pGrassIndexBuffer;
    std::unique_ptr<RENDER::VertexShader> m_pGrassVertexShader;
    std::unique_ptr<RENDER::PixelShader> m_pGrassPixelShader;
    std::unique_ptr<RENDER::InputLayout> m_pGrassInputLayout;
    std::unique_ptr<RENDER::Texture> m_pGrassTexture;
    std::unique_ptr<RENDER::Sampler> m_pGrassSampler;
    std::unique_ptr<RENDER::Blender> m_pBlendStateEnabled;
    std::unique_ptr<RENDER::Blender> m_pBlendStateDisabled;
    std::unique_ptr<RENDER::Rasterizer> m_pRasterizerNoCull;
    std::unique_ptr<RENDER::Rasterizer> m_pRasterizerCull;

    std::unique_ptr<RENDER::ConstantBuffer<CB_transform>> m_pGrassTransformCB;

    DirectX::XMFLOAT3 m_grassPosition = { 0.0f, 0.0f, -1.0f };
    DirectX::XMFLOAT3 m_grassRotation = { 0.0f, 0.0f, 0.0f };
    DirectX::XMFLOAT3 m_grassScale = { 1.0f, 1.0f, 1.0f };

    // Floor
    std::unique_ptr<RENDER::VertexBuffer<Vertex>> m_pFloorVertexBuffer;
    std::unique_ptr<RENDER::IndexBuffer> m_pFloorIndexBuffer;
    std::unique_ptr<RENDER::VertexShader> m_pFloorVertexShader;
    std::unique_ptr<RENDER::PixelShader> m_pFloorPixelShader;
    std::unique_ptr<RENDER::InputLayout> m_pFloorInputLayout;
    std::unique_ptr<RENDER::Texture> m_pFloorTexture;
    std::unique_ptr<RENDER::Sampler> m_pFloorSampler;

    std::unique_ptr<RENDER::ConstantBuffer<CB_transform>> m_pFloorTransformCB;

    DirectX::XMFLOAT3 m_floorPosition = { 0.0f, -0.5f, 0.0f };
    DirectX::XMFLOAT3 m_floorRotation = { 0.0f, 0.0f, 0.0f };
    DirectX::XMFLOAT3 m_floorScale = { 1.0f, 1.0f, 1.0f };
};

}  // end namespace SD::ENGINE
