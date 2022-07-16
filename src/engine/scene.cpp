#include "scene.hpp"

#include <iostream>
#include <math.h>

#include "application.hpp"
#include "exceptions.hpp"


namespace SD::ENGINE {

using namespace RENDER;

Scene::Scene()
{
    std::clog << "Scene initialisation!" << std::endl;
    m_pTimer = std::make_unique<Timer>();

    m_pTimer->GetDelta();
    Setup();
    std::clog << "Scene loaded: " << m_pTimer->GetDelta() << " s." << std::endl;
}

Scene::~Scene()
{
    std::clog << "Scene destroying!" << std::endl;
}

void Scene::Setup()
{
    SetupBox();
    SetupLight();
    SetupFloor();

    SetupGrass();
}

void Scene::Simulate(float dt)
{
    m_simulationTime += dt;
    
    SimulateBox(dt);
    SimulateLight(dt);
    SimulateGrass(dt);
    SimulateFloor(dt);
}

void Scene::Update(float dt)
{
    UpdateBox(dt);
    UpdateLight(dt);
    UpdateGrass(dt);
    UpdateFloor(dt);
}

void Scene::Draw()
{
    DrawBox();
    DrawLight();
    DrawFloor();

    DrawGrass(); // Alpha blending
}

void Scene::SetupBox() {
    const auto& app = Application::GetApplication();
    const auto& renderer = app->GetRenderer();

    // create vertex buffer
    const std::vector<Vertex3> vertices =
    {
        // positions             // normals              // texture coords
        {{ 0.5f, -0.5f, -0.5f},  { 0.0f,  0.0f, -1.0f},  {1.0f, 0.0f}},
        {{-0.5f, -0.5f, -0.5f},  { 0.0f,  0.0f, -1.0f},  {0.0f, 0.0f}},
        {{ 0.5f,  0.5f, -0.5f},  { 0.0f,  0.0f, -1.0f},  {1.0f, 1.0f}},
        {{-0.5f,  0.5f, -0.5f},  { 0.0f,  0.0f, -1.0f},  {0.0f, 1.0f}},
        {{ 0.5f,  0.5f, -0.5f},  { 0.0f,  0.0f, -1.0f},  {1.0f, 1.0f}},
        {{-0.5f, -0.5f, -0.5f},  { 0.0f,  0.0f, -1.0f},  {0.0f, 0.0f}},

        {{-0.5f, -0.5f,  0.5f},  { 0.0f,  0.0f,  1.0f},  {0.0f, 0.0f}},
        {{ 0.5f, -0.5f,  0.5f},  { 0.0f,  0.0f,  1.0f},  {1.0f, 0.0f}},
        {{ 0.5f,  0.5f,  0.5f},  { 0.0f,  0.0f,  1.0f},  {1.0f, 1.0f}},
        {{ 0.5f,  0.5f,  0.5f},  { 0.0f,  0.0f,  1.0f},  {1.0f, 1.0f}},
        {{-0.5f,  0.5f,  0.5f},  { 0.0f,  0.0f,  1.0f},  {0.0f, 1.0f}},
        {{-0.5f, -0.5f,  0.5f},  { 0.0f,  0.0f,  1.0f},  {0.0f, 0.0f}},

        {{-0.5f,  0.5f,  0.5f},  {-1.0f,  0.0f,  0.0f},  {1.0f, 0.0f}},
        {{-0.5f,  0.5f, -0.5f},  {-1.0f,  0.0f,  0.0f},  {1.0f, 1.0f}},
        {{-0.5f, -0.5f, -0.5f},  {-1.0f,  0.0f,  0.0f},  {0.0f, 1.0f}},
        {{-0.5f, -0.5f, -0.5f},  {-1.0f,  0.0f,  0.0f},  {0.0f, 1.0f}},
        {{-0.5f, -0.5f,  0.5f},  {-1.0f,  0.0f,  0.0f},  {0.0f, 0.0f}},
        {{-0.5f,  0.5f,  0.5f},  {-1.0f,  0.0f,  0.0f},  {1.0f, 0.0f}},

        {{ 0.5f,  0.5f, -0.5f},  { 1.0f,  0.0f,  0.0f},  {1.0f, 1.0f}},
        {{ 0.5f,  0.5f,  0.5f},  { 1.0f,  0.0f,  0.0f},  {1.0f, 0.0f}},
        {{ 0.5f, -0.5f, -0.5f},  { 1.0f,  0.0f,  0.0f},  {0.0f, 1.0f}},
        {{ 0.5f, -0.5f,  0.5f},  { 1.0f,  0.0f,  0.0f},  {0.0f, 0.0f}},
        {{ 0.5f, -0.5f, -0.5f},  { 1.0f,  0.0f,  0.0f},  {0.0f, 1.0f}},
        {{ 0.5f,  0.5f,  0.5f},  { 1.0f,  0.0f,  0.0f},  {1.0f, 0.0f}},

        {{-0.5f, -0.5f, -0.5f},  { 0.0f, -1.0f,  0.0f},  {0.0f, 1.0f}},
        {{ 0.5f, -0.5f, -0.5f},  { 0.0f, -1.0f,  0.0f},  {1.0f, 1.0f}},
        {{ 0.5f, -0.5f,  0.5f},  { 0.0f, -1.0f,  0.0f},  {1.0f, 0.0f}},
        {{ 0.5f, -0.5f,  0.5f},  { 0.0f, -1.0f,  0.0f},  {1.0f, 0.0f}},
        {{-0.5f, -0.5f,  0.5f},  { 0.0f, -1.0f,  0.0f},  {0.0f, 0.0f}},
        {{-0.5f, -0.5f, -0.5f},  { 0.0f, -1.0f,  0.0f},  {0.0f, 1.0f}},

        {{ 0.5f,  0.5f, -0.5f},  {0.0f,  1.0f,  0.0f},  {1.0f, 1.0f}},
        {{-0.5f,  0.5f, -0.5f},  {0.0f,  1.0f,  0.0f},  {0.0f, 1.0f}},
        {{ 0.5f,  0.5f,  0.5f},  {0.0f,  1.0f,  0.0f},  {1.0f, 0.0f}},
        {{-0.5f,  0.5f,  0.5f},  {0.0f,  1.0f,  0.0f},  {0.0f, 0.0f}},
        {{ 0.5f,  0.5f,  0.5f},  {0.0f,  1.0f,  0.0f},  {1.0f, 0.0f}},
        {{-0.5f,  0.5f, -0.5f},  {0.0f,  1.0f,  0.0f},  {0.0f, 1.0f}},
    };
    m_pBoxVertexBuffer = std::make_unique<VertexBuffer<Vertex3>>(renderer, vertices);

    // create index buffer
    const std::vector<unsigned short> indices =
    {
        0,  1,  2,    3,  4,  5,
        6,  7,  8,    9,  10, 11,
        12, 13, 14,   15, 16, 17,
        18, 19, 20,   21, 22, 23,
        24, 25, 26,   27, 28, 29,
        30, 31, 32,   33, 34, 35,
    };
    m_pBoxIndexBuffer = std::make_unique<IndexBuffer>(renderer, indices);

    // create shaders
    m_pBoxVertexShader = std::make_unique<VertexShader>(renderer, L"blinn_phong.vs.cso");
    m_pBoxPixelShader = std::make_unique<PixelShader>(renderer, L"blinn_phong.ps.cso");

    // create input (vertex) layout
    const std::vector<D3D11_INPUT_ELEMENT_DESC> inputLayoutDesc =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
    m_pBoxInputLayout = std::make_unique<InputLayout>(renderer, inputLayoutDesc, m_pBoxVertexShader->GetBytecode());

    // create textures
    m_pBoxDiffuseTexture = std::make_unique<Texture>(renderer, L"box.png");
    m_pBoxSpecularTexture = std::make_unique<Texture>(renderer, L"box_specular.png");

    // create texture sampler
    m_pBoxSampler = std::make_unique<Sampler>(renderer);

    // create constant buffers
    CB_transform transformCB;
    m_pBoxTransformCB = std::make_unique<ConstantBuffer<CB_transform>>(renderer, transformCB);

    CB_material materialCB;
    materialCB.shiness = 32.0f;
    m_pBoxMaterialCB = std::make_unique<ConstantBuffer<CB_material>>(renderer, materialCB);
}

void Scene::SetupLight() {
    const auto& app = Application::GetApplication();
    const auto& renderer = app->GetRenderer();

    // create vertex buffer
    const std::vector<Vertex2> vertices =
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
    m_pLightVertexBuffer = std::make_unique<VertexBuffer<Vertex2>>(renderer, vertices);

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
    m_pLightIndexBuffer = std::make_unique<IndexBuffer>(renderer, indices);

    // create shaders
    m_pLightVertexShader = std::make_unique<VertexShader>(renderer, L"light.vs.cso");
    m_pLightPixelShader = std::make_unique<PixelShader>(renderer, L"light.ps.cso");

    // create input (vertex) layout
    const std::vector<D3D11_INPUT_ELEMENT_DESC> inputLayoutDesc =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
    m_pLightInputLayout = std::make_unique<InputLayout>(renderer, inputLayoutDesc, m_pLightVertexShader->GetBytecode());

    // create constant buffers
    CB_transform transformCB;
    m_pLightTransformCB = std::make_unique<ConstantBuffer<CB_transform>>(renderer, transformCB);

    CB_color colorCB;
    colorCB.color = m_lightColor;
    m_pLightColorCB = std::make_unique<ConstantBuffer<CB_color>>(renderer, colorCB);

    CB_posLight posLightCB;
    posLightCB.ambient = { 0.2f, 0.2f, 0.2f };
    posLightCB.diffuse = { 0.5f, 0.5f, 0.5f };
    posLightCB.specular = { 1.0f, 1.0f, 1.0f };
    posLightCB.attenuation = { 1.0f, 0.09f, 0.032f };
    m_pPosLightCB = std::make_unique<ConstantBuffer<CB_posLight>>(renderer, posLightCB);

    CB_dirLight dirLightCB;
    dirLightCB.direction = { 2.0f, 8.0f, 3.0f };
    dirLightCB.ambient = { 0.05f, 0.05f, 0.05f };
    dirLightCB.diffuse = { 0.1f, 0.1f, 0.1f };
    dirLightCB.specular = { 0.5f, 0.5f, 0.5f };
    m_pDirLightCB = std::make_unique<ConstantBuffer<CB_dirLight>>(renderer, dirLightCB);

    CB_spotLight spotLightCB;
    spotLightCB.ambient = { 0.0f, 0.0f, 0.0f };
    spotLightCB.diffuse = { 0.5f, 0.5f, 0.5f };
    spotLightCB.specular = { 1.0f, 1.0f, 1.0f };
    spotLightCB.attenuation = { 1.0f, 0.09f, 0.032f };
    spotLightCB.cutOff = { std::cosf(DirectX::XMConvertToRadians(12.5f)), std::cosf(DirectX::XMConvertToRadians(15.0f)) };
    m_pSpotLightCB = std::make_unique<ConstantBuffer<CB_spotLight>>(renderer, spotLightCB);
}

void Scene::SetupGrass() {
    const auto& app = Application::GetApplication();
    const auto& renderer = app->GetRenderer();

    // create vertex buffer
    const std::vector<Vertex> vertices =
    {
        {{ -0.5f, -0.5f,  0.0f }, {0.0f, 1.0f}},
        {{  0.5f, -0.5f,  0.0f }, {1.0f, 1.0f}},
        {{ -0.5f,  0.5f,  0.0f }, {0.0f, 0.0f}},
        {{  0.5f,  0.5f,  0.0f }, {1.0f, 0.0f}},
    };
    m_pGrassVertexBuffer = std::make_unique<VertexBuffer<Vertex>>(renderer, vertices);

    // create index buffer
    const std::vector<unsigned short> indices =
    {
        0, 2, 1,  2, 3, 1,
    };
    m_pGrassIndexBuffer = std::make_unique<IndexBuffer>(renderer, indices);

    // create shaders
    m_pGrassVertexShader = std::make_unique<VertexShader>(renderer, L"texture.vs.cso");
    m_pGrassPixelShader = std::make_unique<PixelShader>(renderer, L"texture.ps.cso");

    // create input (vertex) layout
    const std::vector<D3D11_INPUT_ELEMENT_DESC> inputLayoutDesc =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
    m_pGrassInputLayout = std::make_unique<InputLayout>(renderer, inputLayoutDesc, m_pGrassVertexShader->GetBytecode());

    // create textures
    m_pGrassTexture = std::make_unique<Texture>(renderer, L"grass.png");

    // create texture sampler
    m_pGrassSampler = std::make_unique<Sampler>(renderer);

    // create constant buffers
    CB_transform transformCB;
    m_pGrassTransformCB = std::make_unique<ConstantBuffer<CB_transform>>(renderer, transformCB);

    // create blend states
    m_pBlendStateEnabled = std::make_unique<Blender>(renderer, true);
    m_pBlendStateDisabled = std::make_unique<Blender>(renderer, false);

    // create rasterizer states
    m_pRasterizerNoCull = std::make_unique<Rasterizer>(renderer, false);
    m_pRasterizerCull = std::make_unique<Rasterizer>(renderer, true);
}

void Scene::SetupFloor()
{
    const auto& app = Application::GetApplication();
    const auto& renderer = app->GetRenderer();

    // create vertex buffer
    const std::vector<Vertex3> vertices =
    {
        {{ -20.0f, 0.0f, -20.0f }, { 0.0f, 1.0f, 0.0f }, { 0.0f,  10.0f }},
        {{  20.0f, 0.0f, -20.0f }, { 0.0f, 1.0f, 0.0f }, { 10.0f, 10.0f }},
        {{ -20.0f, 0.0f,  20.0f }, { 0.0f, 1.0f, 0.0f }, { 0.0f,  0.0f }},
        {{  20.0f, 0.0f,  20.0f }, { 0.0f, 1.0f, 0.0f }, { 10.0f, 0.0f }},
    };
    m_pFloorVertexBuffer = std::make_unique<VertexBuffer<Vertex3>>(renderer, vertices);

    // create index buffer
    const std::vector<unsigned short> indices =
    {
        0, 2, 1,  2, 3, 1,
    };
    m_pFloorIndexBuffer = std::make_unique<IndexBuffer>(renderer, indices);

    // create shaders
    m_pFloorVertexShader = std::make_unique<VertexShader>(renderer, L"blinn_phong.vs.cso");
    m_pFloorPixelShader = std::make_unique<PixelShader>(renderer, L"blinn_phong.ps.cso");

    // create input (vertex) layout
    const std::vector<D3D11_INPUT_ELEMENT_DESC> inputLayoutDesc =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
    m_pFloorInputLayout = std::make_unique<InputLayout>(app->GetRenderer(), inputLayoutDesc, m_pFloorVertexShader->GetBytecode());

    // create textures
    m_pFloorDiffuseTexture = std::make_unique<Texture>(renderer, L"wood.png");
    m_pFloorSpecularTexture = std::make_unique<Texture>(renderer, L"wood_specular.png");

    // create texture sampler
    m_pFloorSampler = std::make_unique<Sampler>(renderer);

    // create constant buffers
    CB_transform transformCB;
    m_pFloorTransformCB = std::make_unique<ConstantBuffer<CB_transform>>(renderer, transformCB);

    CB_material materialCB;
    materialCB.shiness = 32.0f;
    m_pFloorMaterialCB = std::make_unique<ConstantBuffer<CB_material>>(renderer, materialCB);
}

void Scene::SimulateBox(float /* dt */)
{

}

void Scene::SimulateLight(float dt)
{
    m_lightRotation.x += dt;
    m_lightRotation.y += dt;
    m_lightPosition.x = std::sinf(m_simulationTime) * 3.0f;
    m_lightPosition.z = std::cosf(m_simulationTime) * 3.0f;
}

void Scene::SimulateGrass(float dt)
{
    m_grassRotation.x += dt * 0.5f;
}

void Scene::SimulateFloor(float /* dt */)
{

}

void Scene::UpdateBox(float /* dt */)
{
    const auto& app = Application::GetApplication();
    const auto& camera = app->GetCamera();
    const auto& renderer = app->GetRenderer();

    const auto& transformCB = m_pBoxTransformCB->GetData();
    transformCB->model = GetModelMatrix(m_boxPosition, m_boxRotation, m_boxScale);
    transformCB->view = camera->getView();
    transformCB->projection = camera->getProjection();
    transformCB->viewPosition = camera->getPosition();
    m_pBoxTransformCB->Update(renderer);
}

void Scene::UpdateLight(float dt)
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
    spotLightCB->enabled = GetKeyState('F') < 0 ? 1 : 0;  // Is "F" pressed
    m_pSpotLightCB->Update(renderer);
}

void Scene::UpdateGrass(float dt)
{
    const auto& app = Application::GetApplication();
    const auto& camera = app->GetCamera();
    const auto& renderer = app->GetRenderer();

    const auto& transformCB = m_pGrassTransformCB->GetData();
    transformCB->model = GetModelMatrix(m_grassPosition, m_grassRotation, m_grassScale);
    transformCB->view = camera->getView();
    transformCB->projection = camera->getProjection();
    transformCB->viewPosition = camera->getPosition();
    m_pGrassTransformCB->Update(renderer);
}

void Scene::UpdateFloor(float /* dt */)
{
    const auto& app = Application::GetApplication();
    const auto& camera = app->GetCamera();
    const auto& renderer = app->GetRenderer();

    const auto& transformCB = m_pFloorTransformCB->GetData();
    transformCB->model = GetModelMatrix(m_floorPosition, m_floorRotation, m_floorScale);
    transformCB->view = camera->getView();
    transformCB->projection = camera->getProjection();
    transformCB->viewPosition = camera->getPosition();
    m_pFloorTransformCB->Update(renderer);
}

void Scene::DrawBox()
{
    const auto& app = Application::GetApplication();
    const auto& renderer = app->GetRenderer();
    const auto& context = renderer->GetContext();

    D3D_DEBUG_LAYER(renderer);

    // Bind vertex buffer
    m_pBoxVertexBuffer->Bind(renderer);

    // Bind index buffer
    m_pBoxIndexBuffer->Bind(renderer);

    // bind shaders
    m_pBoxVertexShader->Bind(renderer);
    m_pBoxPixelShader->Bind(renderer);

    // bind constant buffers
    m_pBoxTransformCB->VSBind(renderer, 0u);
    m_pBoxMaterialCB->PSBind(renderer, 0u);
    m_pPosLightCB->PSBind(renderer, 1u);
    m_pDirLightCB->PSBind(renderer, 2u);
    m_pSpotLightCB->PSBind(renderer, 3u);

    // bind textures
    m_pBoxDiffuseTexture->Bind(renderer, 0u);
    m_pBoxSpecularTexture->Bind(renderer, 1u);

    // bind texture sampler
    m_pBoxSampler->Bind(renderer);

    // bind vertex layout
    m_pBoxInputLayout->Bind(renderer);

    // Draw
    D3D_THROW_IF_INFO(context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
    D3D_THROW_IF_INFO(context->DrawIndexed(m_pBoxIndexBuffer->GetIndicesCount(), 0u, 0u));
}

void Scene::DrawLight()
{
    const auto& app = Application::GetApplication();
    const auto& renderer = app->GetRenderer();
    const auto& context = renderer->GetContext();

    D3D_DEBUG_LAYER(renderer);

    // Bind vertex buffer
    m_pLightVertexBuffer->Bind(renderer);

    // Bind index buffer
    m_pLightIndexBuffer->Bind(renderer);

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
    D3D_THROW_IF_INFO(context->DrawIndexed(m_pLightIndexBuffer->GetIndicesCount(), 0u, 0u));
}

void Scene::DrawGrass()
{
    const auto& app = Application::GetApplication();
    const auto& renderer = app->GetRenderer();
    const auto& context = renderer->GetContext();

    D3D_DEBUG_LAYER(renderer);

    // Bind vertex buffer
    m_pGrassVertexBuffer->Bind(renderer);

    // Bind index buffer
    m_pGrassIndexBuffer->Bind(renderer);

    // bind shaders
    m_pGrassVertexShader->Bind(renderer);
    m_pGrassPixelShader->Bind(renderer);

    // bind constant buffers
    m_pGrassTransformCB->VSBind(renderer, 0u);

    // bind textures
    m_pGrassTexture->Bind(renderer, 0u);

    // bind texture sampler
    m_pGrassSampler->Bind(renderer);

    // bind vertex layout
    m_pGrassInputLayout->Bind(renderer);

    // Enable alpha blending
    m_pBlendStateEnabled->Bind(renderer);

    // Disable backface culling
    m_pRasterizerNoCull->Bind(renderer);

    // Draw
    D3D_THROW_IF_INFO(context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
    D3D_THROW_IF_INFO(context->DrawIndexed(m_pGrassIndexBuffer->GetIndicesCount(), 0u, 0u));

    // Disable alpha blending
    m_pBlendStateDisabled->Bind(renderer);

    // Enable backface culling
    m_pRasterizerCull->Bind(renderer);
}

void Scene::DrawFloor()
{
    const auto& app = Application::GetApplication();
    const auto& renderer = app->GetRenderer();
    const auto& context = app->GetRenderer()->GetContext();

    D3D_DEBUG_LAYER(renderer);

    // Bind vertex buffer
    m_pFloorVertexBuffer->Bind(renderer);

    // Bind index buffer
    m_pFloorIndexBuffer->Bind(renderer);

    // bind shaders
    m_pFloorVertexShader->Bind(renderer);
    m_pFloorPixelShader->Bind(renderer);

    // bind constant buffers
    m_pFloorTransformCB->VSBind(renderer, 0u);
    m_pFloorMaterialCB->PSBind(renderer, 0u);

    // bind textures
    m_pFloorDiffuseTexture->Bind(renderer, 0u);
    m_pFloorSpecularTexture->Bind(renderer, 1u);

    // bind texture sampler
    m_pFloorSampler->Bind(renderer);

    // bind vertex layout
    m_pFloorInputLayout->Bind(renderer);

    // Draw
    D3D_THROW_IF_INFO(context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
    D3D_THROW_IF_INFO(context->DrawIndexed(m_pFloorIndexBuffer->GetIndicesCount(), 0u, 0u));
}

DirectX::XMMATRIX Scene::GetModelMatrix(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& rotation, const DirectX::XMFLOAT3& scale) const
{
    return DirectX::XMMatrixScaling(scale.x, scale.y, scale.z)*
        DirectX::XMMatrixRotationRollPitchYaw(rotation.y, rotation.x, rotation.z)*
        DirectX::XMMatrixTranslation(position.x, position.y, position.z);
}

}  // end namespace SD::ENGINE
