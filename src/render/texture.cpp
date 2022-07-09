#include "texture.hpp"

#include "exceptions.hpp"


Texture::Texture(Renderer* renderer, const std::wstring& name)
{
    D3D_DEBUG_LAYER(renderer);

    dx::ScratchImage scratch = Load(TEXTURES_PATH + name);
    const auto width = static_cast<UINT>(scratch.GetMetadata().width);
    const auto height = static_cast<UINT>(scratch.GetMetadata().height);
    const auto rowPitch = static_cast<UINT>(scratch.GetImage(0, 0, 0)->rowPitch);

    D3D11_TEXTURE2D_DESC textureDesc = {};
    textureDesc.Width = width;
    textureDesc.Height = height;
    textureDesc.MipLevels = 1;
    textureDesc.ArraySize = 1;
    textureDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    textureDesc.SampleDesc.Count = 1;
    textureDesc.SampleDesc.Quality = 0;
    textureDesc.Usage = D3D11_USAGE_IMMUTABLE;
    textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    textureDesc.CPUAccessFlags = 0;
    textureDesc.MiscFlags = 0;

    D3D11_SUBRESOURCE_DATA textureData = {};
    textureData.pSysMem = scratch.GetPixels();
    textureData.SysMemPitch = rowPitch;

    wrl::ComPtr<ID3D11Texture2D> pDiffuseTexture;
    D3D_THROW_INFO_EXCEPTION(renderer->GetDevice()->CreateTexture2D(&textureDesc, &textureData, pDiffuseTexture.GetAddressOf()));

    D3D11_SHADER_RESOURCE_VIEW_DESC textureSRVDesc = {};
    textureSRVDesc.Format = textureDesc.Format;
    textureSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    textureSRVDesc.Texture2D.MostDetailedMip = 0;
    textureSRVDesc.Texture2D.MipLevels = 1;
    D3D_THROW_INFO_EXCEPTION(renderer->GetDevice()->CreateShaderResourceView(pDiffuseTexture.Get(), &textureSRVDesc, m_pTextureView.GetAddressOf()));
}

void Texture::Bind(Renderer* renderer, UINT slot)
{
    D3D_DEBUG_LAYER(renderer);

    D3D_THROW_IF_INFO(renderer->GetContext()->PSSetShaderResources(slot, 1u, m_pTextureView.GetAddressOf()));
}

dx::ScratchImage Texture::Load(const std::wstring& name)
{
    DirectX::ScratchImage scratch;
    WIN_THROW_IF_FAILED(dx::LoadFromWICFile(name.c_str(), DirectX::WIC_FLAGS_IGNORE_SRGB, nullptr, scratch));

    if (scratch.GetImage(0, 0, 0)->format != DXGI_FORMAT::DXGI_FORMAT_B8G8R8A8_UNORM)
    {
        DirectX::ScratchImage converted;

        WIN_THROW_IF_FAILED(dx::Convert(
            *scratch.GetImage(0, 0, 0),
            DXGI_FORMAT::DXGI_FORMAT_B8G8R8A8_UNORM,
            DirectX::TEX_FILTER_DEFAULT,
            DirectX::TEX_THRESHOLD_DEFAULT,
            converted
        ));

        return converted;
    }

    return scratch;
}
