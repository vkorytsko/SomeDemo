#include "scene.hpp"

#include <d3dcompiler.h>
#include <DirectXTex.h>

#include <iostream>
#include <cmath>

#include "application.hpp"
#include "exceptions.hpp"


dx::ScratchImage loadImage(const wchar_t* name);


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
    const auto& device = app->GetRenderer().GetDevice();

    const Vertex3 vertices[] =
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

    D3D11_BUFFER_DESC vbd = {};
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbd.Usage = D3D11_USAGE_IMMUTABLE;
    vbd.CPUAccessFlags = 0u;
    vbd.MiscFlags = 0u;
    vbd.ByteWidth = sizeof(vertices);
    vbd.StructureByteStride = sizeof(Vertex3);
    D3D11_SUBRESOURCE_DATA vsd = {};
    vsd.pSysMem = vertices;
    D3D_THROW_INFO_EXCEPTION(device->CreateBuffer(&vbd, &vsd, &m_pBoxVertexBuffer));

    // create index buffer
    const unsigned short indices[] =
    {
        0,  1,  2,    3,  4,  5,
        6,  7,  8,    9,  10, 11,
        12, 13, 14,   15, 16, 17,
        18, 19, 20,   21, 22, 23,
        24, 25, 26,   27, 28, 29,
        30, 31, 32,   33, 34, 35,
    };

    D3D11_BUFFER_DESC ibd = {};
    ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibd.Usage = D3D11_USAGE_IMMUTABLE;
    ibd.CPUAccessFlags = 0u;
    ibd.MiscFlags = 0u;
    ibd.ByteWidth = sizeof(indices);
    ibd.StructureByteStride = sizeof(unsigned short);
    D3D11_SUBRESOURCE_DATA isd = {};
    isd.pSysMem = indices;
    D3D_THROW_INFO_EXCEPTION(device->CreateBuffer(&ibd, &isd, &m_pBoxIndexBuffer));

#ifndef NDEBUG  // WTF?!
    const std::wstring path = L"Debug\\";
#else
    const std::wstring path = L"Release\\";
#endif

    // create shaders
    wrl::ComPtr<ID3DBlob> pBlob;
    D3D_THROW_INFO_EXCEPTION(D3DReadFileToBlob((path + L"phong.ps.cso").c_str(), &pBlob));
    D3D_THROW_INFO_EXCEPTION(device->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &m_pBoxPixelShader));
    D3D_THROW_INFO_EXCEPTION(D3DReadFileToBlob((path + L"phong.vs.cso").c_str(), &pBlob));
    D3D_THROW_INFO_EXCEPTION(device->CreateVertexShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &m_pBoxVertexShader));

    // input (vertex) layout
    const D3D11_INPUT_ELEMENT_DESC ied[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
    D3D_THROW_INFO_EXCEPTION(device->CreateInputLayout(
        ied, static_cast<UINT>(std::size(ied)),
        pBlob->GetBufferPointer(), pBlob->GetBufferSize(),
        &m_pBoxInputLayout
    ));

    // load diffuse texture
    dx::ScratchImage dtScratch = loadImage(L"../res/textures/box.png");
    const auto dtWidth = static_cast<UINT>(dtScratch.GetMetadata().width);
    const auto dtHeight = static_cast<UINT>(dtScratch.GetMetadata().height);
    const auto dtRowPitch = static_cast<UINT>(dtScratch.GetImage(0, 0, 0)->rowPitch);

    // create diffuse texture resource
    D3D11_TEXTURE2D_DESC dtd = {};
    dtd.Width = dtWidth;
    dtd.Height = dtHeight;
    dtd.MipLevels = 1;
    dtd.ArraySize = 1;
    dtd.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    dtd.SampleDesc.Count = 1;
    dtd.SampleDesc.Quality = 0;
    dtd.Usage = D3D11_USAGE_IMMUTABLE;
    dtd.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    dtd.CPUAccessFlags = 0;
    dtd.MiscFlags = 0;
    D3D11_SUBRESOURCE_DATA dtsd = {};
    dtsd.pSysMem = dtScratch.GetPixels();
    dtsd.SysMemPitch = dtRowPitch;
    wrl::ComPtr<ID3D11Texture2D> pDiffuseTexture;
    D3D_THROW_INFO_EXCEPTION(device->CreateTexture2D(&dtd, &dtsd, &pDiffuseTexture));

    // create the resource view on the diffuse texture
    D3D11_SHADER_RESOURCE_VIEW_DESC dtSrvDesc = {};
    dtSrvDesc.Format = dtd.Format;
    dtSrvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    dtSrvDesc.Texture2D.MostDetailedMip = 0;
    dtSrvDesc.Texture2D.MipLevels = 1;
    D3D_THROW_INFO_EXCEPTION(device->CreateShaderResourceView(pDiffuseTexture.Get(), &dtSrvDesc, &m_pBoxDiffuseTextureView));

    // load specular texture
    dx::ScratchImage stScratch = loadImage(L"../res/textures/box_specular.png");
    const auto stWidth = static_cast<UINT>(stScratch.GetMetadata().width);
    const auto stHeight = static_cast<UINT>(stScratch.GetMetadata().height);
    const auto stRowPitch = static_cast<UINT>(stScratch.GetImage(0, 0, 0)->rowPitch);

    // create diffuse texture resource
    D3D11_TEXTURE2D_DESC std = {};
    std.Width = stWidth;
    std.Height = stHeight;
    std.MipLevels = 1;
    std.ArraySize = 1;
    std.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    std.SampleDesc.Count = 1;
    std.SampleDesc.Quality = 0;
    std.Usage = D3D11_USAGE_IMMUTABLE;
    std.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    std.CPUAccessFlags = 0;
    std.MiscFlags = 0;
    D3D11_SUBRESOURCE_DATA stsd = {};
    stsd.pSysMem = stScratch.GetPixels();
    stsd.SysMemPitch = stRowPitch;
    wrl::ComPtr<ID3D11Texture2D> pSpecularTexture;
    D3D_THROW_INFO_EXCEPTION(device->CreateTexture2D(&std, &stsd, &pSpecularTexture));

    // create the resource view on the diffuse texture
    D3D11_SHADER_RESOURCE_VIEW_DESC stSrvDesc = {};
    stSrvDesc.Format = std.Format;
    stSrvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    stSrvDesc.Texture2D.MostDetailedMip = 0;
    stSrvDesc.Texture2D.MipLevels = 1;
    D3D_THROW_INFO_EXCEPTION(device->CreateShaderResourceView(pSpecularTexture.Get(), &stSrvDesc, &m_pBoxSpecularTextureView));

    // create texture sampler
    D3D11_SAMPLER_DESC samplerDesc = {};
    samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    D3D_THROW_INFO_EXCEPTION(device->CreateSamplerState(&samplerDesc, &m_pBoxSampler));
}

void Scene::SetupLight() {
    const auto& app = Application::GetApplication();
    const auto& device = app->GetRenderer().GetDevice();

    const Vertex2 vertices[] =
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

    D3D11_BUFFER_DESC vbd = {};
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbd.Usage = D3D11_USAGE_IMMUTABLE;
    vbd.CPUAccessFlags = 0u;
    vbd.MiscFlags = 0u;
    vbd.ByteWidth = sizeof(vertices);
    vbd.StructureByteStride = sizeof(Vertex2);
    D3D11_SUBRESOURCE_DATA vsd = {};
    vsd.pSysMem = vertices;
    D3D_THROW_INFO_EXCEPTION(device->CreateBuffer(&vbd, &vsd, &m_pLightVertexBuffer));

    // create index buffer
    const unsigned short indices[] =
    {
        0, 2, 1,  2, 3, 1,
        1, 3, 5,  3, 7, 5,
        2, 6, 3,  3, 6, 7,
        4, 5, 7,  4, 7, 6,
        0, 4, 2,  2, 4, 6,
        0, 1, 4,  1, 5, 4,
    };

    D3D11_BUFFER_DESC ibd = {};
    ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibd.Usage = D3D11_USAGE_IMMUTABLE;
    ibd.CPUAccessFlags = 0u;
    ibd.MiscFlags = 0u;
    ibd.ByteWidth = sizeof(indices);
    ibd.StructureByteStride = sizeof(unsigned short);
    D3D11_SUBRESOURCE_DATA isd = {};
    isd.pSysMem = indices;
    D3D_THROW_INFO_EXCEPTION(device->CreateBuffer(&ibd, &isd, &m_pLightIndexBuffer));

#ifndef NDEBUG  // WTF?!
    const std::wstring path = L"Debug\\";
#else
    const std::wstring path = L"Release\\";
#endif

    // create shaders
    wrl::ComPtr<ID3DBlob> pBlob;
    D3D_THROW_INFO_EXCEPTION(D3DReadFileToBlob((path + L"light.ps.cso").c_str(), &pBlob));
    D3D_THROW_INFO_EXCEPTION(device->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &m_pLightPixelShader));
    D3D_THROW_INFO_EXCEPTION(D3DReadFileToBlob((path + L"light.vs.cso").c_str(), &pBlob));
    D3D_THROW_INFO_EXCEPTION(device->CreateVertexShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &m_pLightVertexShader));

    // input (vertex) layout
    const D3D11_INPUT_ELEMENT_DESC ied[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
    D3D_THROW_INFO_EXCEPTION(device->CreateInputLayout(
        ied, static_cast<UINT>(std::size(ied)),
        pBlob->GetBufferPointer(), pBlob->GetBufferSize(),
        &m_pLightInputLayout
    ));
}

void Scene::SetupGrass() {
    const auto& app = Application::GetApplication();
    const auto& device = app->GetRenderer().GetDevice();

    const Vertex vertices[] =
    {
        {{ -0.5f, -0.5f,  0.0f }, {0.0f, 1.0f}},
        {{  0.5f, -0.5f,  0.0f }, {1.0f, 1.0f}},
        {{ -0.5f,  0.5f,  0.0f }, {0.0f, 0.0f}},
        {{  0.5f,  0.5f,  0.0f }, {1.0f, 0.0f}},
    };

    D3D11_BUFFER_DESC vbd = {};
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbd.Usage = D3D11_USAGE_IMMUTABLE;
    vbd.CPUAccessFlags = 0u;
    vbd.MiscFlags = 0u;
    vbd.ByteWidth = sizeof(vertices);
    vbd.StructureByteStride = sizeof(Vertex2);
    D3D11_SUBRESOURCE_DATA vsd = {};
    vsd.pSysMem = vertices;
    D3D_THROW_INFO_EXCEPTION(device->CreateBuffer(&vbd, &vsd, &m_pGrassVertexBuffer));

    // create index buffer
    const unsigned short indices[] =
    {
        0, 2, 1,
        2, 3, 1,
    };

    D3D11_BUFFER_DESC ibd = {};
    ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibd.Usage = D3D11_USAGE_IMMUTABLE;
    ibd.CPUAccessFlags = 0u;
    ibd.MiscFlags = 0u;
    ibd.ByteWidth = sizeof(indices);
    ibd.StructureByteStride = sizeof(unsigned short);
    D3D11_SUBRESOURCE_DATA isd = {};
    isd.pSysMem = indices;
    D3D_THROW_INFO_EXCEPTION(device->CreateBuffer(&ibd, &isd, &m_pGrassIndexBuffer));

#ifndef NDEBUG  // WTF?!
    const std::wstring path = L"Debug\\";
#else
    const std::wstring path = L"Release\\";
#endif

    // create shaders
    wrl::ComPtr<ID3DBlob> pBlob;
    D3D_THROW_INFO_EXCEPTION(D3DReadFileToBlob((path + L"texture.ps.cso").c_str(), &pBlob));
    D3D_THROW_INFO_EXCEPTION(device->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &m_pGrassPixelShader));
    D3D_THROW_INFO_EXCEPTION(D3DReadFileToBlob((path + L"texture.vs.cso").c_str(), &pBlob));
    D3D_THROW_INFO_EXCEPTION(device->CreateVertexShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &m_pGrassVertexShader));

    // input (vertex) layout
    const D3D11_INPUT_ELEMENT_DESC ied[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
    D3D_THROW_INFO_EXCEPTION(device->CreateInputLayout(
        ied, static_cast<UINT>(std::size(ied)),
        pBlob->GetBufferPointer(), pBlob->GetBufferSize(),
        &m_pGrassInputLayout
    ));

    dx::ScratchImage scratch = loadImage(L"../res/textures/grass.png");
    const auto textureWidth = static_cast<UINT>(scratch.GetMetadata().width);
    const auto textureHeight = static_cast<UINT>(scratch.GetMetadata().height);
    const auto rowPitch = static_cast<UINT>(scratch.GetImage(0, 0, 0)->rowPitch);

    // create texture resource
    D3D11_TEXTURE2D_DESC td = {};
    td.Width = textureWidth;
    td.Height = textureHeight;
    td.MipLevels = 1;
    td.ArraySize = 1;
    td.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    td.SampleDesc.Count = 1;
    td.SampleDesc.Quality = 0;
    td.Usage = D3D11_USAGE_IMMUTABLE;
    td.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    td.CPUAccessFlags = 0;
    td.MiscFlags = 0;
    D3D11_SUBRESOURCE_DATA tsd = {};
    tsd.pSysMem = scratch.GetPixels();
    tsd.SysMemPitch = rowPitch;
    wrl::ComPtr<ID3D11Texture2D> pTexture;
    D3D_THROW_INFO_EXCEPTION(device->CreateTexture2D(&td, &tsd, &pTexture));

    // create the resource view on the texture
    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Format = td.Format;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MostDetailedMip = 0;
    srvDesc.Texture2D.MipLevels = 1;
    D3D_THROW_INFO_EXCEPTION(device->CreateShaderResourceView(pTexture.Get(), &srvDesc, &m_pGrassTextureView));

    // create texture sampler
    D3D11_SAMPLER_DESC samplerDesc = {};
    samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    D3D_THROW_INFO_EXCEPTION(device->CreateSamplerState(&samplerDesc, &m_pGrassSampler));

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
    const auto& device = app->GetRenderer().GetDevice();

    const Vertex vertices[] =
    {
        {{ -20.0f, 0.0f, -20.0f }, {0.0f,  10.0f}},
        {{  20.0f, 0.0f, -20.0f }, {10.0f, 10.0f}},
        {{ -20.0f, 0.0f,  20.0f }, {0.0f,  0.0f}},
        {{  20.0f, 0.0f,  20.0f }, {10.0f, 0.0f}},
    };

    D3D11_BUFFER_DESC vbd = {};
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbd.Usage = D3D11_USAGE_IMMUTABLE;
    vbd.CPUAccessFlags = 0u;
    vbd.MiscFlags = 0u;
    vbd.ByteWidth = sizeof(vertices);
    vbd.StructureByteStride = sizeof(Vertex2);
    D3D11_SUBRESOURCE_DATA vsd = {};
    vsd.pSysMem = vertices;
    D3D_THROW_INFO_EXCEPTION(device->CreateBuffer(&vbd, &vsd, &m_pFloorVertexBuffer));

    // create index buffer
    const unsigned short indices[] =
    {
        0, 2, 1,
        2, 3, 1,
    };

    D3D11_BUFFER_DESC ibd = {};
    ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibd.Usage = D3D11_USAGE_IMMUTABLE;
    ibd.CPUAccessFlags = 0u;
    ibd.MiscFlags = 0u;
    ibd.ByteWidth = sizeof(indices);
    ibd.StructureByteStride = sizeof(unsigned short);
    D3D11_SUBRESOURCE_DATA isd = {};
    isd.pSysMem = indices;
    D3D_THROW_INFO_EXCEPTION(device->CreateBuffer(&ibd, &isd, &m_pFloorIndexBuffer));

#ifndef NDEBUG  // WTF?!
    const std::wstring path = L"Debug\\";
#else
    const std::wstring path = L"Release\\";
#endif

    // create shaders
    wrl::ComPtr<ID3DBlob> pBlob;
    D3D_THROW_INFO_EXCEPTION(D3DReadFileToBlob((path + L"texture.ps.cso").c_str(), &pBlob));
    D3D_THROW_INFO_EXCEPTION(device->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &m_pFloorPixelShader));
    D3D_THROW_INFO_EXCEPTION(D3DReadFileToBlob((path + L"texture.vs.cso").c_str(), &pBlob));
    D3D_THROW_INFO_EXCEPTION(device->CreateVertexShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &m_pFloorVertexShader));

    // input (vertex) layout
    const D3D11_INPUT_ELEMENT_DESC ied[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
    D3D_THROW_INFO_EXCEPTION(device->CreateInputLayout(
        ied, static_cast<UINT>(std::size(ied)),
        pBlob->GetBufferPointer(), pBlob->GetBufferSize(),
        &m_pFloorInputLayout
    ));

    dx::ScratchImage scratch = loadImage(L"../res/textures/marble.jpg");
    const auto textureWidth = static_cast<UINT>(scratch.GetMetadata().width);
    const auto textureHeight = static_cast<UINT>(scratch.GetMetadata().height);
    const auto rowPitch = static_cast<UINT>(scratch.GetImage(0, 0, 0)->rowPitch);

    // create texture resource
    D3D11_TEXTURE2D_DESC td = {};
    td.Width = textureWidth;
    td.Height = textureHeight;
    td.MipLevels = 1;
    td.ArraySize = 1;
    td.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    td.SampleDesc.Count = 1;
    td.SampleDesc.Quality = 0;
    td.Usage = D3D11_USAGE_IMMUTABLE;
    td.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    td.CPUAccessFlags = 0;
    td.MiscFlags = 0;
    D3D11_SUBRESOURCE_DATA tsd = {};
    tsd.pSysMem = scratch.GetPixels();
    tsd.SysMemPitch = rowPitch;
    wrl::ComPtr<ID3D11Texture2D> pTexture;
    D3D_THROW_INFO_EXCEPTION(device->CreateTexture2D(&td, &tsd, &pTexture));

    // create the resource view on the texture
    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Format = td.Format;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MostDetailedMip = 0;
    srvDesc.Texture2D.MipLevels = 1;
    D3D_THROW_INFO_EXCEPTION(device->CreateShaderResourceView(pTexture.Get(), &srvDesc, &m_pFloorTextureView));

    // create texture sampler
    D3D11_SAMPLER_DESC samplerDesc = {};
    samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    D3D_THROW_INFO_EXCEPTION(device->CreateSamplerState(&samplerDesc, &m_pFloorSampler));
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
    const auto& device = app->GetRenderer().GetDevice();
    const auto& context = app->GetRenderer().GetContext();

    // create constant buffers
    CB_transform transformCB;
    transformCB.model = dx::XMMatrixScaling(m_boxScale.x, m_boxScale.y, m_boxScale.z) *
        dx::XMMatrixRotationRollPitchYaw(m_boxRotation.y, m_boxRotation.x, m_boxRotation.z) *
        dx::XMMatrixTranslation(m_boxPosition.x, m_boxPosition.y, m_boxPosition.z);
    transformCB.view = camera.getView();
    transformCB.projection = camera.getProjection();
    transformCB.viewPosition = camera.getPosition();

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
    const UINT stride = sizeof(Vertex3);
    const UINT offset = 0u;
    D3D_THROW_IF_INFO(context->IASetVertexBuffers(0u, 1u, m_pBoxVertexBuffer.GetAddressOf(), &stride, &offset));

    // Bind vertex buffer
    D3D_THROW_IF_INFO(context->IASetIndexBuffer(m_pBoxIndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0u));

    // bind shaders
    D3D_THROW_IF_INFO(context->VSSetShader(m_pBoxVertexShader.Get(), nullptr, 0u));
    D3D_THROW_IF_INFO(context->PSSetShader(m_pBoxPixelShader.Get(), nullptr, 0u));

    // bind constant buffers
    D3D_THROW_IF_INFO(context->VSSetConstantBuffers(0u, 1u, pTransformCB.GetAddressOf()));
    D3D_THROW_IF_INFO(context->PSSetConstantBuffers(0u, 1u, pMaterialCB.GetAddressOf()))
    D3D_THROW_IF_INFO(context->PSSetConstantBuffers(1u, 1u, pPosLightCB.GetAddressOf()))
    D3D_THROW_IF_INFO(context->PSSetConstantBuffers(2u, 1u, pDirLightCB.GetAddressOf()))

    // bind texture to pixel shader
    D3D_THROW_IF_INFO(context->PSSetShaderResources(0u, 1u, m_pBoxDiffuseTextureView.GetAddressOf()));
    D3D_THROW_IF_INFO(context->PSSetShaderResources(1u, 1u, m_pBoxSpecularTextureView.GetAddressOf()));

    // bind texture sampler to pixel shader
    D3D_THROW_IF_INFO(context->PSSetSamplers(0, 1, m_pBoxSampler.GetAddressOf()));

    // bind vertex layout
    D3D_THROW_IF_INFO(context->IASetInputLayout(m_pBoxInputLayout.Get()));

    // Set primitive topology to triangle list (groups of 3 vertices)
    D3D_THROW_IF_INFO(context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));

    D3D_THROW_IF_INFO(context->DrawIndexed(36u, 0u, 0u));
}

void Scene::DrawLight()
{
    const auto& app = Application::GetApplication();
    const auto& camera = app->GetCamera();
    const auto& device = app->GetRenderer().GetDevice();
    const auto& context = app->GetRenderer().GetContext();

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
                camera.getView() *
                camera.getProjection()
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
    const UINT stride = sizeof(Vertex2);
    const UINT offset = 0u;
    D3D_THROW_IF_INFO(context->IASetVertexBuffers(0u, 1u, m_pLightVertexBuffer.GetAddressOf(), &stride, &offset));

    // Bind vertex buffer
    D3D_THROW_IF_INFO(context->IASetIndexBuffer(m_pLightIndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0u));

    // bind shaders
    D3D_THROW_IF_INFO(context->VSSetShader(m_pLightVertexShader.Get(), nullptr, 0u));
    D3D_THROW_IF_INFO(context->PSSetShader(m_pLightPixelShader.Get(), nullptr, 0u));

    // bind constant buffer to vertex shader
    D3D_THROW_IF_INFO(context->VSSetConstantBuffers(0u, 1u, pConstantBuffer.GetAddressOf()));

    // bind vertex layout
    D3D_THROW_IF_INFO(context->IASetInputLayout(m_pLightInputLayout.Get()));

    // Set primitive topology to triangle list (groups of 3 vertices)
    D3D_THROW_IF_INFO(context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));

    D3D_THROW_IF_INFO(context->DrawIndexed(36u, 0u, 0u));
}

dx::ScratchImage loadImage(const wchar_t* name)
{
    dx::ScratchImage scratch;
    WIN_THROW_IF_FAILED(dx::LoadFromWICFile(name, dx::WIC_FLAGS_IGNORE_SRGB, nullptr, scratch));

    if (scratch.GetImage(0, 0, 0)->format != DXGI_FORMAT::DXGI_FORMAT_B8G8R8A8_UNORM)
    {
        dx::ScratchImage converted;

        WIN_THROW_IF_FAILED(dx::Convert(
            *scratch.GetImage(0, 0, 0),
            DXGI_FORMAT::DXGI_FORMAT_B8G8R8A8_UNORM,
            dx::TEX_FILTER_DEFAULT,
            dx::TEX_THRESHOLD_DEFAULT,
            converted
        ));

        return converted;
    }

    return scratch;
}

void Scene::DrawGrass()
{
    const auto& app = Application::GetApplication();
    const auto& camera = app->GetCamera();
    const auto& device = app->GetRenderer().GetDevice();
    const auto& context = app->GetRenderer().GetContext();

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
                camera.getView() *
                camera.getProjection()
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
    const UINT stride = sizeof(Vertex);
    const UINT offset = 0u;
    D3D_THROW_IF_INFO(context->IASetVertexBuffers(0u, 1u, m_pGrassVertexBuffer.GetAddressOf(), &stride, &offset));

    // Bind vertex buffer
    D3D_THROW_IF_INFO(context->IASetIndexBuffer(m_pGrassIndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0u));

    // bind shaders
    D3D_THROW_IF_INFO(context->VSSetShader(m_pGrassVertexShader.Get(), nullptr, 0u));
    D3D_THROW_IF_INFO(context->PSSetShader(m_pGrassPixelShader.Get(), nullptr, 0u));

    // bind constant buffer to vertex shader
    D3D_THROW_IF_INFO(context->VSSetConstantBuffers(0u, 1u, pConstantBuffer.GetAddressOf()));

    // bind texture to pixel shader
    D3D_THROW_IF_INFO(context->PSSetShaderResources(0u, 1u, m_pGrassTextureView.GetAddressOf()));

    // bind texture sampler to pixel shader
    D3D_THROW_IF_INFO(context->PSSetSamplers(0, 1, m_pGrassSampler.GetAddressOf()));

    // bind vertex layout
    D3D_THROW_IF_INFO(context->IASetInputLayout(m_pGrassInputLayout.Get()));

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
    const auto& device = app->GetRenderer().GetDevice();
    const auto& context = app->GetRenderer().GetContext();

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
                camera.getView() *
                camera.getProjection()
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
    const UINT stride = sizeof(Vertex);
    const UINT offset = 0u;
    D3D_THROW_IF_INFO(context->IASetVertexBuffers(0u, 1u, m_pFloorVertexBuffer.GetAddressOf(), &stride, &offset));

    // Bind vertex buffer
    D3D_THROW_IF_INFO(context->IASetIndexBuffer(m_pFloorIndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0u));

    // bind shaders
    D3D_THROW_IF_INFO(context->VSSetShader(m_pFloorVertexShader.Get(), nullptr, 0u));
    D3D_THROW_IF_INFO(context->PSSetShader(m_pFloorPixelShader.Get(), nullptr, 0u));

    // bind constant buffer to vertex shader
    D3D_THROW_IF_INFO(context->VSSetConstantBuffers(0u, 1u, pConstantBuffer.GetAddressOf()));

    // bind texture to pixel shader
    D3D_THROW_IF_INFO(context->PSSetShaderResources(0u, 1u, m_pFloorTextureView.GetAddressOf()));

    // bind texture sampler to pixel shader
    D3D_THROW_IF_INFO(context->PSSetSamplers(0, 1, m_pFloorSampler.GetAddressOf()));

    // bind vertex layout
    D3D_THROW_IF_INFO(context->IASetInputLayout(m_pFloorInputLayout.Get()));

    // Set primitive topology to triangle list (groups of 3 vertices)
    D3D_THROW_IF_INFO(context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));

    D3D_THROW_IF_INFO(context->DrawIndexed(6u, 0u, 0u));
}
