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
    const auto& renderSystem = app->GetRenderSystem();

    SetupLight();

    m_pRasterizer = std::make_unique<RENDER::Rasterizer>(renderSystem->GetRenderer(), true);
    m_pBlender = std::make_unique<RENDER::Blender>(renderSystem->GetRenderer(), false);
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
    const auto& renderSystem = app->GetRenderSystem();

    m_pRasterizer->Bind(renderSystem->GetRenderer());
    m_pBlender->Bind(renderSystem->GetRenderer());

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
    const auto& renderSystem = app->GetRenderSystem();

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
    m_pLightVertexBuffer->create(renderSystem->GetRenderer(), vertices.data(), m_lightVertexBufferStride * vertices.size());

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
    m_pLightIndexBuffer->create(renderSystem->GetRenderer(), indices.data(), m_lightIndexBufferStride * indices.size());

    // create shaders
    m_pLightVertexShader = std::make_unique<RENDER::VertexShader>(renderSystem->GetRenderer(), L"light.vs.cso");
    m_pLightPixelShader = std::make_unique<RENDER::PixelShader>(renderSystem->GetRenderer(), L"light.ps.cso");

    // create input (vertex) layout
    const std::vector<D3D11_INPUT_ELEMENT_DESC> inputLayoutDesc =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
    m_pLightInputLayout = std::make_unique<RENDER::InputLayout>(renderSystem->GetRenderer(), inputLayoutDesc, m_pLightVertexShader->GetBytecode());

    // create constant buffers
    CB_transform transformCB;
    m_pLightTransformCB = std::make_unique<RENDER::ConstantBuffer<CB_transform>>(renderSystem->GetRenderer(), transformCB);

    CB_color colorCB;
    colorCB.color = m_lightColor;
    m_pLightColorCB = std::make_unique<RENDER::ConstantBuffer<CB_color>>(renderSystem->GetRenderer(), colorCB);

    CB_posLight posLightCB;
    posLightCB.ambient = { 0.4f, 0.4f, 0.4f };
    posLightCB.diffuse = { 0.8f, 0.8f, 0.8f };
    posLightCB.specular = { 1.0f, 1.0f, 1.0f };
    posLightCB.attenuation = { 1.0f, 0.09f, 0.032f };
    m_pPosLightCB = std::make_unique<RENDER::ConstantBuffer<CB_posLight>>(renderSystem->GetRenderer(), posLightCB);

    CB_dirLight dirLightCB;
    dirLightCB.direction = { 2.0f, 8.0f, 3.0f };
    dirLightCB.ambient = { 0.05f, 0.05f, 0.05f };
    dirLightCB.diffuse = { 0.1f, 0.1f, 0.1f };
    dirLightCB.specular = { 0.5f, 0.5f, 0.5f };
    m_pDirLightCB = std::make_unique<RENDER::ConstantBuffer<CB_dirLight>>(renderSystem->GetRenderer(), dirLightCB);

    CB_spotLight spotLightCB;
    spotLightCB.ambient = { 0.0f, 0.0f, 0.0f };
    spotLightCB.diffuse = { 0.5f, 0.5f, 0.5f };
    spotLightCB.specular = { 1.0f, 1.0f, 1.0f };
    spotLightCB.attenuation = { 1.0f, 0.09f, 0.032f };
    spotLightCB.cutOff = { std::cosf(DirectX::XMConvertToRadians(12.5f)), std::cosf(DirectX::XMConvertToRadians(15.0f)) };
    m_pSpotLightCB = std::make_unique<RENDER::ConstantBuffer<CB_spotLight>>(renderSystem->GetRenderer(), spotLightCB);
}

void DebugWorld::SimulateLight(float dt)
{
    m_lightRotation.x += dt;
    m_lightRotation.y += dt;
    m_lightPosition.x = std::sinf(m_space->simulationTime()) * 6.0f;
}

void DebugWorld::UpdateLight(float)
{
    const auto& app = Application::GetApplication();
    const auto& camera = app->GetCamera();
    const auto& renderSystem = app->GetRenderSystem();

    const auto& transformCB = m_pLightTransformCB->GetData();
    transformCB->model = GetModelMatrix(m_lightPosition, m_lightRotation, m_lightScale);
    transformCB->view = camera->getView();
    transformCB->projection = camera->getProjection();
    transformCB->viewPosition = camera->getPosition();
    m_pLightTransformCB->Update(renderSystem->GetRenderer());

    const auto& posLightCB = m_pPosLightCB->GetData();
    posLightCB->position = m_lightPosition;
    m_pPosLightCB->Update(renderSystem->GetRenderer());

    const auto& spotLightCB = m_pSpotLightCB->GetData();
    spotLightCB->position = camera->getPosition();
    spotLightCB->direction = camera->getDirection();
    spotLightCB->enabled = IsSpotLightEnabled() ? 1 : 0;
    m_pSpotLightCB->Update(renderSystem->GetRenderer());
}

void DebugWorld::DrawLight()
{
    const auto& app = Application::GetApplication();
    const auto& renderSystem = app->GetRenderSystem();
    const auto& context = renderSystem->GetRenderer()->GetContext();

    D3D_DEBUG_LAYER(renderSystem->GetRenderer());

    // Bind vertex buffer
    m_pLightVertexBuffer->Bind(renderSystem->GetRenderer(), 0u, static_cast<UINT>(m_lightVertexBufferStride), 0u);

    // Bind index buffer
    m_pLightIndexBuffer->Bind(renderSystem->GetRenderer(), 0u, static_cast<UINT>(m_lightIndexBufferStride), 0u);

    // bind shaders
    m_pLightVertexShader->Bind(renderSystem->GetRenderer());
    m_pLightPixelShader->Bind(renderSystem->GetRenderer());

    // bind constant buffers
    m_pLightTransformCB->VSBind(renderSystem->GetRenderer(), 0u);
    m_pLightColorCB->PSBind(renderSystem->GetRenderer(), 0u);

    // bind vertex layout
    m_pLightInputLayout->Bind(renderSystem->GetRenderer());

    // Draw
    D3D_THROW_IF_INFO(context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
    D3D_THROW_IF_INFO(context->DrawIndexed(static_cast<UINT>(m_lightIndicesCount), 0u, 0u));
}

void DebugWorld::BindLights()
{
    const auto& app = Application::GetApplication();
    const auto& renderSystem = app->GetRenderSystem();

    m_pPosLightCB->PSBind(renderSystem->GetRenderer(), 1u);
    m_pDirLightCB->PSBind(renderSystem->GetRenderer(), 2u);
    m_pSpotLightCB->PSBind(renderSystem->GetRenderer(), 3u);
}

bool DebugWorld::IsSpotLightEnabled()
{
    return GetKeyState('F') < 0;
}
}  // end namespace SD::ENGINE
