#include "scene.hpp"

#include <iostream>
#include <cmath>

#include "application.hpp"
#include "exceptions.hpp"


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

void Scene::Update(float dt)
{
    UpdateBox(dt);
    UpdateLight(dt);
    UpdateFloor(dt);

    UpdateGrass(dt);
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
    const auto& device = app->GetRenderer()->GetDevice();

    D3D_DEBUG_LAYER(app->GetRenderer());

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

    m_pBoxVertexBuffer = std::make_unique<VertexBuffer>(app->GetRenderer(), vertices);

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
    m_pBoxIndexBuffer = std::make_unique<IndexBuffer>(app->GetRenderer(), indices);

    // create shaders
    m_pBoxVertexShader = std::make_unique<VertexShader>(app->GetRenderer(), L"phong.vs.cso");
    m_pBoxPixelShader = std::make_unique<PixelShader>(app->GetRenderer(), L"phong.ps.cso");

    // input (vertex) layout
    const std::vector<D3D11_INPUT_ELEMENT_DESC> inputLayoutDesc =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
    m_pBoxInputLayout = std::make_unique<InputLayout>(app->GetRenderer(), inputLayoutDesc, m_pBoxVertexShader->GetBytecode());

    // create textures
    m_pBoxDiffuseTexture = std::make_unique<Texture>(app->GetRenderer(), L"box.png");
    m_pBoxSpecularTexture = std::make_unique<Texture>(app->GetRenderer(), L"box_specular.png");

    // create texture sampler
    m_pBoxSampler = std::make_unique<Sampler>(app->GetRenderer());
}

void Scene::SetupLight() {
    const auto& app = Application::GetApplication();
    const auto& device = app->GetRenderer()->GetDevice();

    D3D_DEBUG_LAYER(app->GetRenderer());

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
    m_pLightVertexBuffer = std::make_unique<VertexBuffer>(app->GetRenderer(), vertices);

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
    m_pLightIndexBuffer = std::make_unique<IndexBuffer>(app->GetRenderer(), indices);

    // create shaders
    m_pLightVertexShader = std::make_unique<VertexShader>(app->GetRenderer(), L"light.vs.cso");
    m_pLightPixelShader = std::make_unique<PixelShader>(app->GetRenderer(), L"light.ps.cso");

    // input (vertex) layout
    const std::vector<D3D11_INPUT_ELEMENT_DESC> inputLayoutDesc =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
    m_pLightInputLayout = std::make_unique<InputLayout>(app->GetRenderer(), inputLayoutDesc, m_pLightVertexShader->GetBytecode());
}

void Scene::SetupGrass() {
    const auto& app = Application::GetApplication();
    const auto& device = app->GetRenderer()->GetDevice();

    D3D_DEBUG_LAYER(app->GetRenderer());

    const std::vector<Vertex> vertices =
    {
        {{ -0.5f, -0.5f,  0.0f }, {0.0f, 1.0f}},
        {{  0.5f, -0.5f,  0.0f }, {1.0f, 1.0f}},
        {{ -0.5f,  0.5f,  0.0f }, {0.0f, 0.0f}},
        {{  0.5f,  0.5f,  0.0f }, {1.0f, 0.0f}},
    };
    m_pGrassVertexBuffer = std::make_unique<VertexBuffer>(app->GetRenderer(), vertices);

    // create index buffer
    const std::vector<unsigned short> indices =
    {
        0, 2, 1,  2, 3, 1,
    };
    m_pGrassIndexBuffer = std::make_unique<IndexBuffer>(app->GetRenderer(), indices);

    // create shaders
    m_pGrassVertexShader = std::make_unique<VertexShader>(app->GetRenderer(), L"texture.vs.cso");
    m_pGrassPixelShader = std::make_unique<PixelShader>(app->GetRenderer(), L"texture.ps.cso");

    // input (vertex) layout
    const std::vector<D3D11_INPUT_ELEMENT_DESC> inputLayoutDesc =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
    m_pGrassInputLayout = std::make_unique<InputLayout>(app->GetRenderer(), inputLayoutDesc, m_pGrassVertexShader->GetBytecode());

    // create texture
    m_pGrassTexture = std::make_unique<Texture>(app->GetRenderer(), L"grass.png");

    // create texture sampler
    m_pGrassSampler = std::make_unique<Sampler>(app->GetRenderer());

    // create blend states
    // blending enabled
    D3D11_BLEND_DESC blendDescEnabled = {};
    auto& brte = blendDescEnabled.RenderTarget[0];
    brte.BlendEnable = TRUE;
    brte.SrcBlend = D3D11_BLEND_SRC_ALPHA;
    brte.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
    brte.BlendOp = D3D11_BLEND_OP_ADD;
    brte.SrcBlendAlpha = D3D11_BLEND_ZERO;
    brte.DestBlendAlpha = D3D11_BLEND_ZERO;
    brte.BlendOpAlpha = D3D11_BLEND_OP_ADD;
    brte.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
    D3D_THROW_IF_INFO(device->CreateBlendState(&blendDescEnabled, &m_pBlendStateEnabled));
    //blending disabled
    D3D11_BLEND_DESC blendDescDisabled = {};
    auto& brtd = blendDescDisabled.RenderTarget[0];
    brtd.BlendEnable = FALSE;
    brtd.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
    D3D_THROW_IF_INFO(device->CreateBlendState(&blendDescDisabled, &m_pBlendStateDisabled));

    // create rasterizer states
    // backface culling disabled
    D3D11_RASTERIZER_DESC rasterDescNoCull = CD3D11_RASTERIZER_DESC(CD3D11_DEFAULT{});
    rasterDescNoCull.CullMode = D3D11_CULL_NONE;
    D3D_THROW_IF_INFO(device->CreateRasterizerState(&rasterDescNoCull, &m_pRasterizerNoCull));
    // backfase culling enabled
    D3D11_RASTERIZER_DESC rasterDescCull = CD3D11_RASTERIZER_DESC(CD3D11_DEFAULT{});
    rasterDescCull.CullMode = D3D11_CULL_BACK;
    D3D_THROW_IF_INFO(device->CreateRasterizerState(&rasterDescCull, &m_pRasterizerCull));
}

void Scene::SetupFloor()
{
    const auto& app = Application::GetApplication();
    const auto& device = app->GetRenderer()->GetDevice();

    D3D_DEBUG_LAYER(app->GetRenderer());

    const std::vector<Vertex> vertices =
    {
        {{ -20.0f, 0.0f, -20.0f }, {0.0f,  10.0f}},
        {{  20.0f, 0.0f, -20.0f }, {10.0f, 10.0f}},
        {{ -20.0f, 0.0f,  20.0f }, {0.0f,  0.0f}},
        {{  20.0f, 0.0f,  20.0f }, {10.0f, 0.0f}},
    };
    m_pFloorVertexBuffer = std::make_unique<VertexBuffer>(app->GetRenderer(), vertices);

    // create index buffer
    const std::vector<unsigned short> indices =
    {
        0, 2, 1,  2, 3, 1,
    };
    m_pFloorIndexBuffer = std::make_unique<IndexBuffer>(app->GetRenderer(), indices);

    // create shaders
    m_pFloorVertexShader = std::make_unique<VertexShader>(app->GetRenderer(), L"texture.vs.cso");
    m_pFloorPixelShader = std::make_unique<PixelShader>(app->GetRenderer(), L"texture.ps.cso");

    // input (vertex) layout
    const std::vector<D3D11_INPUT_ELEMENT_DESC> inputLayoutDesc =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
    m_pFloorInputLayout = std::make_unique<InputLayout>(app->GetRenderer(), inputLayoutDesc, m_pFloorVertexShader->GetBytecode());

    // create texture
    m_pFloorTexture = std::make_unique<Texture>(app->GetRenderer(), L"marble.jpg");

    // create texture sampler
    m_pFloorSampler = std::make_unique<Sampler>(app->GetRenderer());
}

void Scene::UpdateBox(float /* dt */)
{

}

void Scene::UpdateLight(float dt)
{
    float time = m_pTimer->GetTotal();
    m_lightRotation.x += dt;
    m_lightRotation.y += dt;
    m_lightPosition.x = std::sinf(time) * 3.0f;
    m_lightPosition.z = std::cosf(time) * 3.0f;
}

void Scene::UpdateGrass(float dt)
{
    m_grassRotation.x += dt * 0.5f;
}

void Scene::UpdateFloor(float /* dt */)
{

}

void Scene::DrawBox()
{
    const auto& app = Application::GetApplication();
    const auto& camera = app->GetCamera();
    const auto& device = app->GetRenderer()->GetDevice();
    const auto& context = app->GetRenderer()->GetContext();

    D3D_DEBUG_LAYER(app->GetRenderer());

    // create constant buffers
    CB_transform transformCB;
    transformCB.model = dx::XMMatrixScaling(m_boxScale.x, m_boxScale.y, m_boxScale.z) *
        dx::XMMatrixRotationRollPitchYaw(m_boxRotation.y, m_boxRotation.x, m_boxRotation.z) *
        dx::XMMatrixTranslation(m_boxPosition.x, m_boxPosition.y, m_boxPosition.z);
    transformCB.view = camera->getView();
    transformCB.projection = camera->getProjection();
    transformCB.viewPosition = camera->getPosition();

    CB_material materialCB;
    materialCB.shiness = 32.0f;

    CB_posLight posLightCB;
    posLightCB.position = m_lightPosition;
    posLightCB.ambient = { 0.2f, 0.2f, 0.2f };
    posLightCB.diffuse = { 0.5f, 0.5f, 0.5f };
    posLightCB.specular = { 1.0f, 1.0f, 1.0f };
    posLightCB.attenuation = { 1.0f, 0.09f, 0.032f };

    CB_dirLight dirLightCB;
    dirLightCB.direction = { 2.0f, 8.0f, 3.0f };
    dirLightCB.ambient = { 0.05f, 0.05f, 0.05f };
    dirLightCB.diffuse = { 0.4f, 0.4f, 0.4f };
    dirLightCB.specular = { 0.5f, 0.5f, 0.5f };

    wrl::ComPtr<ID3D11Buffer> pTransformCB;
    D3D11_BUFFER_DESC tcbd;
    tcbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    tcbd.Usage = D3D11_USAGE_DYNAMIC;
    tcbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    tcbd.MiscFlags = 0u;
    tcbd.ByteWidth = sizeof(transformCB);
    tcbd.StructureByteStride = 0u;
    D3D11_SUBRESOURCE_DATA tcsd = {};
    tcsd.pSysMem = &transformCB;
    D3D_THROW_INFO_EXCEPTION(device->CreateBuffer(&tcbd, &tcsd, &pTransformCB));

    wrl::ComPtr<ID3D11Buffer> pMaterialCB;
    D3D11_BUFFER_DESC mcbd;
    mcbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    mcbd.Usage = D3D11_USAGE_DYNAMIC;
    mcbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    mcbd.MiscFlags = 0u;
    mcbd.ByteWidth = sizeof(materialCB);
    mcbd.StructureByteStride = 0u;
    D3D11_SUBRESOURCE_DATA mcsd = {};
    mcsd.pSysMem = &materialCB;
    D3D_THROW_INFO_EXCEPTION(device->CreateBuffer(&mcbd, &mcsd, &pMaterialCB));

    wrl::ComPtr<ID3D11Buffer> pPosLightCB;
    D3D11_BUFFER_DESC plcbd;
    plcbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    plcbd.Usage = D3D11_USAGE_DYNAMIC;
    plcbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    plcbd.MiscFlags = 0u;
    plcbd.ByteWidth = sizeof(posLightCB);
    plcbd.StructureByteStride = 0u;
    D3D11_SUBRESOURCE_DATA plcsd = {};
    plcsd.pSysMem = &posLightCB;
    D3D_THROW_INFO_EXCEPTION(device->CreateBuffer(&plcbd, &plcsd, &pPosLightCB));

    wrl::ComPtr<ID3D11Buffer> pDirLightCB;
    D3D11_BUFFER_DESC dlcbd;
    dlcbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    dlcbd.Usage = D3D11_USAGE_DYNAMIC;
    dlcbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    dlcbd.MiscFlags = 0u;
    dlcbd.ByteWidth = sizeof(dirLightCB);
    dlcbd.StructureByteStride = 0u;
    D3D11_SUBRESOURCE_DATA dlcsd = {};
    dlcsd.pSysMem = &dirLightCB;
    D3D_THROW_INFO_EXCEPTION(device->CreateBuffer(&dlcbd, &dlcsd, &pDirLightCB));

    // Bind vertex buffer
    m_pBoxVertexBuffer->Bind(app->GetRenderer());

    // Bind vertex buffer
    m_pBoxIndexBuffer->Bind(app->GetRenderer());

    // bind shaders
    m_pBoxVertexShader->Bind(app->GetRenderer());
    m_pBoxPixelShader->Bind(app->GetRenderer());

    // bind constant buffers
    D3D_THROW_IF_INFO(context->VSSetConstantBuffers(0u, 1u, pTransformCB.GetAddressOf()));
    D3D_THROW_IF_INFO(context->PSSetConstantBuffers(0u, 1u, pMaterialCB.GetAddressOf()));
    D3D_THROW_IF_INFO(context->PSSetConstantBuffers(1u, 1u, pPosLightCB.GetAddressOf()));
    D3D_THROW_IF_INFO(context->PSSetConstantBuffers(2u, 1u, pDirLightCB.GetAddressOf()));

    // bind texture to pixel shader
    m_pBoxDiffuseTexture->Bind(app->GetRenderer(), 0u);
    m_pBoxSpecularTexture->Bind(app->GetRenderer(), 1u);

    // bind texture sampler to pixel shader
    m_pBoxSampler->Bind(app->GetRenderer());

    // bind vertex layout
    m_pBoxInputLayout->Bind(app->GetRenderer());

    // Set primitive topology to triangle list (groups of 3 vertices)
    D3D_THROW_IF_INFO(context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));

    D3D_THROW_IF_INFO(context->DrawIndexed(36u, 0u, 0u));
}

void Scene::DrawLight()
{
    const auto& app = Application::GetApplication();
    const auto& camera = app->GetCamera();
    const auto& device = app->GetRenderer()->GetDevice();
    const auto& context = app->GetRenderer()->GetContext();

    D3D_DEBUG_LAYER(app->GetRenderer());

    // create constant buffer
    struct ConstantBuffer
    {
        dx::XMMATRIX transform;
        dx::XMFLOAT3 color;
    };
    const ConstantBuffer cb =
    {
        {
            dx::XMMatrixTranspose(
                dx::XMMatrixScaling(m_lightScale.x, m_lightScale.y, m_lightScale.z)*
                dx::XMMatrixRotationRollPitchYaw(m_lightRotation.y, m_lightRotation.x, m_lightRotation.z) *
                dx::XMMatrixTranslation(m_lightPosition.x, m_lightPosition.y, m_lightPosition.z) *
                camera->getView() *
                camera->getProjection()
            )
        },
        m_lightColor,
    };
    wrl::ComPtr<ID3D11Buffer> pConstantBuffer;
    D3D11_BUFFER_DESC cbd;
    cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cbd.Usage = D3D11_USAGE_DYNAMIC;
    cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    cbd.MiscFlags = 0u;
    cbd.ByteWidth = sizeof(cb);
    cbd.StructureByteStride = 0u;
    D3D11_SUBRESOURCE_DATA csd = {};
    csd.pSysMem = &cb;
    D3D_THROW_INFO_EXCEPTION(device->CreateBuffer(&cbd, &csd, &pConstantBuffer));

    // Bind vertex buffer
    m_pLightVertexBuffer->Bind(app->GetRenderer());

    // Bind vertex buffer
    m_pLightIndexBuffer->Bind(app->GetRenderer());

    // bind shaders
    m_pLightVertexShader->Bind(app->GetRenderer());
    m_pLightPixelShader->Bind(app->GetRenderer());

    // bind constant buffer to vertex shader
    D3D_THROW_IF_INFO(context->VSSetConstantBuffers(0u, 1u, pConstantBuffer.GetAddressOf()));

    // bind vertex layout
    m_pLightInputLayout->Bind(app->GetRenderer());

    // Set primitive topology to triangle list (groups of 3 vertices)
    D3D_THROW_IF_INFO(context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));

    D3D_THROW_IF_INFO(context->DrawIndexed(36u, 0u, 0u));
}

void Scene::DrawGrass()
{
    const auto& app = Application::GetApplication();
    const auto& camera = app->GetCamera();
    const auto& device = app->GetRenderer()->GetDevice();
    const auto& context = app->GetRenderer()->GetContext();

    D3D_DEBUG_LAYER(app->GetRenderer());

    // create constant buffer
    struct ConstantBuffer
    {
        dx::XMMATRIX transform;
    };
    const ConstantBuffer cb =
    {
        {
            dx::XMMatrixTranspose(
                dx::XMMatrixScaling(m_grassScale.x, m_grassScale.y, m_grassScale.z) *
                dx::XMMatrixRotationRollPitchYaw(m_grassRotation.y, m_grassRotation.x, m_grassRotation.z) *
                dx::XMMatrixTranslation(m_grassPosition.x, m_grassPosition.y, m_grassPosition.z) *
                camera->getView() *
                camera->getProjection()
            )
        }
    };
    wrl::ComPtr<ID3D11Buffer> pConstantBuffer;
    D3D11_BUFFER_DESC cbd;
    cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cbd.Usage = D3D11_USAGE_DYNAMIC;
    cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    cbd.MiscFlags = 0u;
    cbd.ByteWidth = sizeof(cb);
    cbd.StructureByteStride = 0u;
    D3D11_SUBRESOURCE_DATA csd = {};
    csd.pSysMem = &cb;
    D3D_THROW_INFO_EXCEPTION(device->CreateBuffer(&cbd, &csd, &pConstantBuffer));

    // Bind vertex buffer
    m_pGrassVertexBuffer->Bind(app->GetRenderer());

    // Bind vertex buffer
    m_pGrassIndexBuffer->Bind(app->GetRenderer());

    // bind shaders
    m_pGrassVertexShader->Bind(app->GetRenderer());
    m_pGrassPixelShader->Bind(app->GetRenderer());

    // bind constant buffer to vertex shader
    D3D_THROW_IF_INFO(context->VSSetConstantBuffers(0u, 1u, pConstantBuffer.GetAddressOf()));

    // bind texture to pixel shader
    m_pGrassTexture->Bind(app->GetRenderer(), 0u);

    // bind texture sampler to pixel shader
    m_pGrassSampler->Bind(app->GetRenderer());

    // bind vertex layout
    m_pGrassInputLayout->Bind(app->GetRenderer());

    // Set primitive topology to triangle list (groups of 3 vertices)
    D3D_THROW_IF_INFO(context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));

    // Enable alpha blending
    D3D_THROW_IF_INFO(context->OMSetBlendState(m_pBlendStateEnabled.Get(), nullptr, 0xFFFFFFFFu));

    // Disable backface culling
    D3D_THROW_IF_INFO(context->RSSetState(m_pRasterizerNoCull.Get()));


    D3D_THROW_IF_INFO(context->DrawIndexed(6u, 0u, 0u));

    // Disable alpha blending
    D3D_THROW_IF_INFO(context->OMSetBlendState(m_pBlendStateDisabled.Get(), nullptr, 0xFFFFFFFFu));

    // Enable backface culling
    D3D_THROW_IF_INFO(context->RSSetState(m_pRasterizerCull.Get()));
}

void Scene::DrawFloor()
{
    const auto& app = Application::GetApplication();
    const auto& camera = app->GetCamera();
    const auto& device = app->GetRenderer()->GetDevice();
    const auto& context = app->GetRenderer()->GetContext();

    D3D_DEBUG_LAYER(app->GetRenderer());

    // create constant buffer
    struct ConstantBuffer
    {
        dx::XMMATRIX transform;
    };
    const ConstantBuffer cb =
    {
        {
            dx::XMMatrixTranspose(
                dx::XMMatrixScaling(m_floorScale.x, m_floorScale.y, m_floorScale.z) *
                dx::XMMatrixRotationRollPitchYaw(m_floorRotation.y, m_floorRotation.x, m_floorRotation.z) *
                dx::XMMatrixTranslation(m_floorPosition.x, m_floorPosition.y, m_floorPosition.z) *
                camera->getView() *
                camera->getProjection()
            )
        }
    };
    wrl::ComPtr<ID3D11Buffer> pConstantBuffer;
    D3D11_BUFFER_DESC cbd;
    cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cbd.Usage = D3D11_USAGE_DYNAMIC;
    cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    cbd.MiscFlags = 0u;
    cbd.ByteWidth = sizeof(cb);
    cbd.StructureByteStride = 0u;
    D3D11_SUBRESOURCE_DATA csd = {};
    csd.pSysMem = &cb;
    D3D_THROW_INFO_EXCEPTION(device->CreateBuffer(&cbd, &csd, &pConstantBuffer));

    // Bind vertex buffer
    m_pFloorVertexBuffer->Bind(app->GetRenderer());

    // Bind vertex buffer
    m_pFloorIndexBuffer->Bind(app->GetRenderer());

    // bind shaders
    m_pFloorVertexShader->Bind(app->GetRenderer());
    m_pFloorPixelShader->Bind(app->GetRenderer());

    // bind constant buffer to vertex shader
    D3D_THROW_IF_INFO(context->VSSetConstantBuffers(0u, 1u, pConstantBuffer.GetAddressOf()));

    // bind texture to pixel shader
    m_pFloorTexture->Bind(app->GetRenderer(), 0u);

    // bind texture sampler to pixel shader
    m_pFloorSampler->Bind(app->GetRenderer());

    // bind vertex layout
    m_pFloorInputLayout->Bind(app->GetRenderer());

    // Set primitive topology to triangle list (groups of 3 vertices)
    D3D_THROW_IF_INFO(context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));

    D3D_THROW_IF_INFO(context->DrawIndexed(6u, 0u, 0u));
}
