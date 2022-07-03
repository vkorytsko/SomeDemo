#include "scene.hpp"

#include <d3dcompiler.h>
#include <DirectXTex.h>

#include <iostream>

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
    const auto& app = Application::GetApplication();
    const auto& device = app->GetRenderer().GetDevice();

    const Vertex vertices[] =
    {
        {{-1.0f, -1.0f, -1.0f}, {2.0f / 3.0f, 0.0f / 4.0f}},
        {{ 1.0f, -1.0f, -1.0f}, {1.0f / 3.0f ,0.0f / 4.0f}},
        {{-1.0f,  1.0f, -1.0f}, {2.0f / 3.0f, 1.0f / 4.0f}},
        {{ 1.0f,  1.0f, -1.0f}, {1.0f / 3.0f, 1.0f / 4.0f}},
        {{-1.0f, -1.0f,  1.0f}, {2.0f / 3.0f, 3.0f / 4.0f}},
        {{ 1.0f, -1.0f,  1.0f}, {1.0f / 3.0f, 3.0f / 4.0f}},
        {{-1.0f,  1.0f,  1.0f}, {2.0f / 3.0f, 2.0f / 4.0f}},
        {{ 1.0f,  1.0f,  1.0f}, {1.0f / 3.0f, 2.0f / 4.0f}},
        {{-1.0f, -1.0f, -1.0f}, {2.0f / 3.0f, 4.0f / 4.0f}},
        {{ 1.0f, -1.0f, -1.0f}, {1.0f / 3.0f, 4.0f / 4.0f}},
        {{-1.0f, -1.0f, -1.0f}, {3.0f / 3.0f, 1.0f / 4.0f}},
        {{-1.0f, -1.0f,  1.0f}, {3.0f / 3.0f, 2.0f / 4.0f}},
        {{ 1.0f, -1.0f, -1.0f}, {0.0f / 3.0f, 1.0f / 4.0f}},
        {{ 1.0f, -1.0f,  1.0f}, {0.0f / 3.0f, 2.0f / 4.0f}},
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
    D3D_THROW_INFO_EXCEPTION(device->CreateBuffer(&vbd, &vsd, &m_pVertexBuffer));

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
    D3D_THROW_INFO_EXCEPTION(device->CreateBuffer(&ibd, &isd, &m_pIndexBuffer));

    // create shaders
    wrl::ComPtr<ID3DBlob> pBlob;
    D3D_THROW_INFO_EXCEPTION(D3DReadFileToBlob(L"Debug/texture.ps.cso", &pBlob));
    D3D_THROW_INFO_EXCEPTION(device->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &m_pPixelShader));
    D3D_THROW_INFO_EXCEPTION(D3DReadFileToBlob(L"Debug/texture.vs.cso", &pBlob));
    D3D_THROW_INFO_EXCEPTION(device->CreateVertexShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &m_pVertexShader));

    // input (vertex) layout
    const D3D11_INPUT_ELEMENT_DESC ied[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
    D3D_THROW_INFO_EXCEPTION(device->CreateInputLayout(
        ied, static_cast<UINT>(std::size(ied)),
        pBlob->GetBufferPointer(), pBlob->GetBufferSize(),
        &m_pInputLayout
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
    D3D_THROW_INFO_EXCEPTION(device->CreateShaderResourceView(pTexture.Get(), &srvDesc, &m_pTextureView));

    // create texture sampler
    D3D11_SAMPLER_DESC samplerDesc = {};
    samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    D3D_THROW_INFO_EXCEPTION(device->CreateSamplerState(&samplerDesc, &m_pSampler));
}

void Scene::Update(float dt)
{
    m_yaw += dt;
    m_pitch += dt;
}

void Scene::Draw()
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
                dx::XMMatrixRotationX(m_pitch) *
                dx::XMMatrixRotationZ(m_yaw) *
                dx::XMMatrixTranslation(0.0f, 0.0f, 5.0f) *
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
    D3D_THROW_IF_INFO(context->IASetVertexBuffers(0u, 1u, m_pVertexBuffer.GetAddressOf(), &stride, &offset));

    // Bind vertex buffer
    D3D_THROW_IF_INFO(context->IASetIndexBuffer(m_pIndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0u));

    // bind shaders
    D3D_THROW_IF_INFO(context->VSSetShader(m_pVertexShader.Get(), nullptr, 0u));
    D3D_THROW_IF_INFO(context->PSSetShader(m_pPixelShader.Get(), nullptr, 0u));

    // bind constant buffer to vertex shader
    D3D_THROW_IF_INFO(context->VSSetConstantBuffers(0u, 1u, pConstantBuffer.GetAddressOf()));

    // bind texture to pixel shader
    D3D_THROW_IF_INFO(context->PSSetShaderResources(0u, 1u, m_pTextureView.GetAddressOf()));

    // bind texture sampler to pixel shader
    D3D_THROW_IF_INFO(context->PSSetSamplers(0, 1, m_pSampler.GetAddressOf()));

    // bind vertex layout
    D3D_THROW_IF_INFO(context->IASetInputLayout(m_pInputLayout.Get()));

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
