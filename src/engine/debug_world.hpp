#pragma once

#include <DirectXMath.h>

#include <memory>

#include "constant_buffer.hpp"
#include "index_buffer.hpp"
#include "input_layout.hpp"
#include "pixel_shader.hpp"
#include "vertex_buffer.hpp"
#include "vertex_shader.hpp"


namespace SD::ENGINE {

class DebugWorld
{
private:
    struct Vertex
    {
        DirectX::XMFLOAT3 pos;
    };

    struct CB_transform
    {
        DirectX::XMMATRIX model;
        DirectX::XMMATRIX view;
        DirectX::XMMATRIX projection;
        alignas(16) DirectX::XMFLOAT3 viewPosition;
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
    DebugWorld();
	~DebugWorld() = default;

	void Setup();

	void Simulate(float dt);
	void Update(float dt);
	void Draw();

	void BindLights();

private:
    void SetupLight();
    void SimulateLight(float dt);
    void UpdateLight(float dt);
    void DrawLight();

    bool IsSpotLightEnabled();

private:
    // Common
    float m_simulationTime = 0.0f;

    // Light
    std::unique_ptr<RENDER::VertexBuffer> m_pLightVertexBuffer;
    size_t m_lightVertexBufferStride = 0;
    std::unique_ptr<RENDER::IndexBuffer> m_pLightIndexBuffer;
    size_t m_lightIndexBufferStride = 0;
    size_t m_lightIndicesCount = 0;
    std::unique_ptr<RENDER::VertexShader> m_pLightVertexShader;
    std::unique_ptr<RENDER::PixelShader> m_pLightPixelShader;
    std::unique_ptr<RENDER::InputLayout> m_pLightInputLayout;

    std::unique_ptr<RENDER::ConstantBuffer<CB_transform>> m_pLightTransformCB;
    std::unique_ptr<RENDER::ConstantBuffer<CB_color>> m_pLightColorCB;
    std::unique_ptr<RENDER::ConstantBuffer<CB_posLight>> m_pPosLightCB;
    std::unique_ptr<RENDER::ConstantBuffer<CB_dirLight>> m_pDirLightCB;
    std::unique_ptr<RENDER::ConstantBuffer<CB_spotLight>> m_pSpotLightCB;

    DirectX::XMFLOAT3 m_lightPosition = { 0.0f, 2.0f, 0.0f };
    DirectX::XMFLOAT3 m_lightRotation = { 0.0f, 0.0f, 0.0f };
    DirectX::XMFLOAT3 m_lightScale = { 0.2f, 0.2f, 0.2f };
    DirectX::XMFLOAT3 m_lightColor = { 1.0f, 1.0f, 1.0f };
};
}  // end namespace SD::ENGINE