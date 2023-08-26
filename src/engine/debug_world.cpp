#include "debug_world.hpp"

#include <cmath>

#include "application.hpp"
#include "exceptions.hpp"

#include <imgui.h>


namespace
{
DirectX::XMMATRIX GetModelMatrix(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& rotation, const DirectX::XMFLOAT3& scale)
{
    return DirectX::XMMatrixScaling(scale.x, scale.y, scale.z) *
        DirectX::XMMatrixRotationRollPitchYaw(rotation.y, rotation.x, rotation.z) *
        DirectX::XMMatrixTranslation(position.x, position.y, position.z);
}
}

namespace SD::ENGINE {

DebugWorld::DebugWorld(const Space* space)
	: m_space(space)
{
    Setup();
}

void DebugWorld::Setup()
{
    const auto& app = Application::GetApplication();
    const auto& renderer = app->GetRenderer();

    SetupLight();

    m_pRasterizer = std::make_unique<RENDER::Rasterizer>(renderer, true);
    m_pBlender = std::make_unique<RENDER::Blender>(renderer, false);
}

void DebugWorld::Simulate(float dt)
{
    SimulateLight(dt);
}

void DebugWorld::Update(float dt)
{
    UpdateLight(dt);
}

void DebugWorld::Draw()
{
    const auto& app = Application::GetApplication();
    const auto& renderer = app->GetRenderer();

    m_pRasterizer->Bind(renderer);
    m_pBlender->Bind(renderer);

    DrawLight();
}

void DebugWorld::DrawImGui()
{
    bool show_demo_window = true;
    if (show_demo_window)
    {
        ImGui::ShowDemoWindow(&show_demo_window);
    }
}

void DebugWorld::SetupLight() {
    const auto& app = Application::GetApplication();
    const auto& renderer = app->GetRenderer();

    // create vertex buffer
    const std::vector<Vertex> vertices =
    {
        {{ -0.5f, -0.5f, -0.5f }},
        {{  0.5f, -0.5f, -0.5f }},
        {{ -0.5f,  0.5f, -0.5f }},
        {{  0.5f,  0.5f, -0.5f }},
        {{ -0.5f, -0.5f,  0.5f }},
        {{  0.5f, -0.5f,  0.5f }},
        {{ -0.5f,  0.5f,  0.5f }},
        {{  0.5f,  0.5f,  0.5f }},
    };
    m_lightVertexBufferStride = sizeof(decltype(vertices)::value_type);
    m_pLightVertexBuffer = std::make_unique<RENDER::VertexBuffer>();
    m_pLightVertexBuffer->create(renderer, vertices.data(), m_lightVertexBufferStride * vertices.size());

    // create index buffer
    const std::vector<unsigned short> indices =
    {
        0, 2, 1,  2, 3, 1,
        1, 3, 5,  3, 7, 5,
        2, 6, 3,  3, 6, 7,
        4, 5, 7,  4, 7, 6,
        0, 4, 2,  2, 4, 6,
        0, 1, 4,  1, 5, 4,
    };
    m_lightIndexBufferStride = sizeof(decltype(indices)::value_type);
    m_lightIndicesCount = indices.size();
    m_pLightIndexBuffer = std::make_unique<RENDER::IndexBuffer>();
    m_pLightIndexBuffer->create(renderer, indices.data(), m_lightIndexBufferStride * indices.size());

    // create shaders
    m_pLightVertexShader = std::make_unique<RENDER::VertexShader>(renderer, L"light.vs.cso");
    m_pLightPixelShader = std::make_unique<RENDER::PixelShader>(renderer, L"light.ps.cso");

    // create input (vertex) layout
    const std::vector<D3D11_INPUT_ELEMENT_DESC> inputLayoutDesc =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
    m_pLightInputLayout = std::make_unique<RENDER::InputLayout>(renderer, inputLayoutDesc, m_pLightVertexShader->GetBytecode());

    // create constant buffers
    CB_transform transformCB;
    m_pLightTransformCB = std::make_unique<RENDER::ConstantBuffer<CB_transform>>(renderer, transformCB);

    CB_color colorCB;
    colorCB.color = m_lightColor;
    m_pLightColorCB = std::make_unique<RENDER::ConstantBuffer<CB_color>>(renderer, colorCB);

    CB_posLight posLightCB;
    posLightCB.ambient = { 0.4f, 0.4f, 0.4f };
    posLightCB.diffuse = { 0.8f, 0.8f, 0.8f };
    posLightCB.specular = { 1.0f, 1.0f, 1.0f };
    posLightCB.attenuation = { 1.0f, 0.09f, 0.032f };
    m_pPosLightCB = std::make_unique<RENDER::ConstantBuffer<CB_posLight>>(renderer, posLightCB);

    CB_dirLight dirLightCB;
    dirLightCB.direction = { 2.0f, 8.0f, 3.0f };
    dirLightCB.ambient = { 0.05f, 0.05f, 0.05f };
    dirLightCB.diffuse = { 0.1f, 0.1f, 0.1f };
    dirLightCB.specular = { 0.5f, 0.5f, 0.5f };
    m_pDirLightCB = std::make_unique<RENDER::ConstantBuffer<CB_dirLight>>(renderer, dirLightCB);

    CB_spotLight spotLightCB;
    spotLightCB.ambient = { 0.0f, 0.0f, 0.0f };
    spotLightCB.diffuse = { 0.5f, 0.5f, 0.5f };
    spotLightCB.specular = { 1.0f, 1.0f, 1.0f };
    spotLightCB.attenuation = { 1.0f, 0.09f, 0.032f };
    spotLightCB.cutOff = { std::cosf(DirectX::XMConvertToRadians(12.5f)), std::cosf(DirectX::XMConvertToRadians(15.0f)) };
    m_pSpotLightCB = std::make_unique<RENDER::ConstantBuffer<CB_spotLight>>(renderer, spotLightCB);
}

void DebugWorld::SimulateLight(float dt)
{
    m_lightRotation.x += dt;
    m_lightRotation.y += dt;
    m_lightPosition.x = std::sinf(m_space->simulationTime()) * 6.0f;
}

void DebugWorld::UpdateLight(float dt)
{
    const auto& app = Application::GetApplication();
    const auto& camera = app->GetCamera();
    const auto& renderer = app->GetRenderer();

    const auto& transformCB = m_pLightTransformCB->GetData();
    transformCB->model = GetModelMatrix(m_lightPosition, m_lightRotation, m_lightScale);
    transformCB->view = camera->getView();
    transformCB->projection = camera->getProjection();
    transformCB->viewPosition = camera->getPosition();
    m_pLightTransformCB->Update(renderer);

    const auto& posLightCB = m_pPosLightCB->GetData();
    posLightCB->position = m_lightPosition;
    m_pPosLightCB->Update(renderer);

    const auto& spotLightCB = m_pSpotLightCB->GetData();
    spotLightCB->position = camera->getPosition();
    spotLightCB->direction = camera->getDirection();
    spotLightCB->enabled = IsSpotLightEnabled() ? 1 : 0;
    m_pSpotLightCB->Update(renderer);
}

void DebugWorld::DrawLight()
{
    const auto& app = Application::GetApplication();
    const auto& renderer = app->GetRenderer();
    const auto& context = renderer->GetContext();

    D3D_DEBUG_LAYER(renderer);

    // Bind vertex buffer
    m_pLightVertexBuffer->Bind(renderer, 0u, static_cast<UINT>(m_lightVertexBufferStride), 0u);

    // Bind index buffer
    m_pLightIndexBuffer->Bind(renderer, 0u, static_cast<UINT>(m_lightIndexBufferStride), 0u);

    // bind shaders
    m_pLightVertexShader->Bind(renderer);
    m_pLightPixelShader->Bind(renderer);

    // bind constant buffers
    m_pLightTransformCB->VSBind(renderer, 0u);
    m_pLightColorCB->PSBind(renderer, 0u);

    // bind vertex layout
    m_pLightInputLayout->Bind(renderer);

    // Draw
    D3D_THROW_IF_INFO(context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
    D3D_THROW_IF_INFO(context->DrawIndexed(m_lightIndicesCount, 0u, 0u));
}

void DebugWorld::BindLights()
{
    const auto& app = Application::GetApplication();
    const auto& renderer = app->GetRenderer();
    const auto& context = renderer->GetContext();

    m_pPosLightCB->PSBind(renderer, 1u);
    m_pDirLightCB->PSBind(renderer, 2u);
    m_pSpotLightCB->PSBind(renderer, 3u);
}

bool DebugWorld::IsSpotLightEnabled()
{
    return GetKeyState('F') < 0;
}
}  // end namespace SD::ENGINE
