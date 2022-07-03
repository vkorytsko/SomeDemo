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
    SetupGrass();
    SetupFloor();
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
    DrawGrass();
    DrawFloor();
}

void Scene::SetupBox() {
    const auto& app = Application::GetApplication();
    const auto& device = app->GetRenderer().GetDevice();

    const Vertex vertices[] =
    {
        {{-0.5f, -0.5f, -0.5f}, {2.0f / 3.0f, 0.0f / 4.0f}},
        {{ 0.5f, -0.5f, -0.5f}, {1.0f / 3.0f ,0.0f / 4.0f}},
        {{-0.5f,  0.5f, -0.5f}, {2.0f / 3.0f, 1.0f / 4.0f}},
        {{ 0.5f,  0.5f, -0.5f}, {1.0f / 3.0f, 1.0f / 4.0f}},
        {{-0.5f, -0.5f,  0.5f}, {2.0f / 3.0f, 3.0f / 4.0f}},
        {{ 0.5f, -0.5f,  0.5f}, {1.0f / 3.0f, 3.0f / 4.0f}},
        {{-0.5f,  0.5f,  0.5f}, {2.0f / 3.0f, 2.0f / 4.0f}},
        {{ 0.5f,  0.5f,  0.5f}, {1.0f / 3.0f, 2.0f / 4.0f}},
        {{-0.5f, -0.5f, -0.5f}, {2.0f / 3.0f, 4.0f / 4.0f}},
        {{ 0.5f, -0.5f, -0.5f}, {1.0f / 3.0f, 4.0f / 4.0f}},
        {{-0.5f, -0.5f, -0.5f}, {3.0f / 3.0f, 1.0f / 4.0f}},
        {{-0.5f, -0.5f,  0.5f}, {3.0f / 3.0f, 2.0f / 4.0f}},
        {{ 0.5f, -0.5f, -0.5f}, {0.0f / 3.0f, 1.0f / 4.0f}},
        {{ 0.5f, -0.5f,  0.5f}, {0.0f / 3.0f, 2.0f / 4.0f}},
    };

    D3D11_BUFFER_DESC vbd = {};
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbd.Usage = D3D11_USAGE_IMMUTABLE;
    vbd.CPUAccessFlags = 0u;
    vbd.MiscFlags = 0u;
    vbd.ByteWidth = sizeof(vertices);
    vbd.StructureByteStride = sizeof(Vertex);
    D3D11_SUBRESOURCE_DATA vsd = {};
    vsd.pSysMem = vertices;
    D3D_THROW_INFO_EXCEPTION(device->CreateBuffer(&vbd, &vsd, &m_pBoxVertexBuffer));

    // create index buffer
    const unsigned short indices[] =
    {
        0,  2,  1,   2,  3,  1,
        4,  8,  5,   5,  8,  9,
        2,  6,  3,   3,  6,  7,
        4,  5,  7,   4,  7,  6,
        2,  10, 11,  2,  11, 6,
        12, 3,  7,   12, 7,  13,
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
    D3D_THROW_INFO_EXCEPTION(D3DReadFileToBlob((path + L"texture.ps.cso").c_str(), &pBlob));
    D3D_THROW_INFO_EXCEPTION(device->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &m_pBoxPixelShader));
    D3D_THROW_INFO_EXCEPTION(D3DReadFileToBlob((path + L"texture.vs.cso").c_str(), &pBlob));
    D3D_THROW_INFO_EXCEPTION(device->CreateVertexShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &m_pBoxVertexShader));

    // input (vertex) layout
    const D3D11_INPUT_ELEMENT_DESC ied[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
    D3D_THROW_INFO_EXCEPTION(device->CreateInputLayout(
        ied, static_cast<UINT>(std::size(ied)),
        pBlob->GetBufferPointer(), pBlob->GetBufferSize(),
        &m_pBoxInputLayout
    ));

    dx::ScratchImage scratch = loadImage(L"../res/textures/cube.png");
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
    D3D_THROW_INFO_EXCEPTION(device->CreateShaderResourceView(pTexture.Get(), &srvDesc, &m_pBoxTextureView));

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
    m_grassRotation.x += dt;
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

    // create constant buffer
    struct ConstantBuffer
    {
        dx::XMMATRIX transform;
    };
    const ConstantBuffer cb =
    {
        {
            dx::XMMatrixTranspose(
                dx::XMMatrixScaling(m_boxScale.x, m_boxScale.y, m_boxScale.z) *
                dx::XMMatrixRotationRollPitchYaw(m_boxRotation.y, m_boxRotation.x, m_boxRotation.z) *
                dx::XMMatrixTranslation(m_boxPosition.x, m_boxPosition.y, m_boxPosition.z) *
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
    D3D_THROW_IF_INFO(context->IASetVertexBuffers(0u, 1u, m_pBoxVertexBuffer.GetAddressOf(), &stride, &offset));

    // Bind vertex buffer
    D3D_THROW_IF_INFO(context->IASetIndexBuffer(m_pBoxIndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0u));

    // bind shaders
    D3D_THROW_IF_INFO(context->VSSetShader(m_pBoxVertexShader.Get(), nullptr, 0u));
    D3D_THROW_IF_INFO(context->PSSetShader(m_pBoxPixelShader.Get(), nullptr, 0u));

    // bind constant buffer to vertex shader
    D3D_THROW_IF_INFO(context->VSSetConstantBuffers(0u, 1u, pConstantBuffer.GetAddressOf()));

    // bind texture to pixel shader
    D3D_THROW_IF_INFO(context->PSSetShaderResources(0u, 1u, m_pBoxTextureView.GetAddressOf()));

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

    D3D_THROW_IF_INFO(context->DrawIndexed(6u, 0u, 0u));
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