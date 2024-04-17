#include "texture.hpp"

#include "renderer.hpp"
#include "debug_layer.hpp"
#include "exceptions.hpp"

#include <DirectXTex.h>


namespace SD::RENDER {

Texture::Texture(Renderer* renderer, const std::wstring& path)
{
    D3D_DEBUG_LAYER(renderer);

    DirectX::ScratchImage scratch = Load(path);

    D3D11_TEXTURE2D_DESC textureDesc = {};
    textureDesc.Width = static_cast<UINT>(scratch.GetMetadata().width);
    textureDesc.Height = static_cast<UINT>(scratch.GetMetadata().height);
    textureDesc.MipLevels = static_cast<UINT>(scratch.GetMetadata().mipLevels);
    textureDesc.ArraySize = static_cast<UINT>(scratch.GetMetadata().arraySize);
    textureDesc.Format = scratch.GetMetadata().format;
    textureDesc.SampleDesc.Count = 1;
    textureDesc.SampleDesc.Quality = 0;
    textureDesc.Usage = D3D11_USAGE_IMMUTABLE;
    textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    textureDesc.CPUAccessFlags = 0;
    textureDesc.MiscFlags = 0;

    std::vector<D3D11_SUBRESOURCE_DATA> initialData;
    initialData.reserve(textureDesc.MipLevels);
    for (auto mip = 0; mip < initialData.capacity(); mip++)
    {
        const auto& i = scratch.GetImage(mip, 0, 0);
        auto& data = initialData.emplace_back();
        data.pSysMem = i->pixels;
        data.SysMemPitch = static_cast<UINT>(i->rowPitch);
    }

    Microsoft::WRL::ComPtr<ID3D11Texture2D> pDiffuseTexture;
    D3D_THROW_INFO_EXCEPTION(renderer->GetDevice()->CreateTexture2D(&textureDesc, initialData.data(), pDiffuseTexture.GetAddressOf()));

    D3D11_SHADER_RESOURCE_VIEW_DESC textureSRVDesc = {};
    textureSRVDesc.Format = textureDesc.Format;
    textureSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    textureSRVDesc.Texture2D.MostDetailedMip = 0;
    textureSRVDesc.Texture2D.MipLevels = static_cast<UINT>(-1);
    D3D_THROW_INFO_EXCEPTION(renderer->GetDevice()->CreateShaderResourceView(pDiffuseTexture.Get(), &textureSRVDesc, m_pTextureView.GetAddressOf()));
}

void Texture::Bind(Renderer* renderer, UINT slot) const
{
    D3D_DEBUG_LAYER(renderer);

    D3D_THROW_IF_INFO(renderer->GetContext()->PSSetShaderResources(slot, 1u, m_pTextureView.GetAddressOf()));
}

DirectX::ScratchImage Texture::Load(const std::wstring& path)
{
    DirectX::ScratchImage image, convert;
    WIN_THROW_IF_FAILED(DirectX::LoadFromDDSFile(path.c_str(), DirectX::DDS_FLAGS_NONE, nullptr, image));

    /*
    if (image.GetImage(0, 0, 0)->format != DXGI_FORMAT::DXGI_FORMAT_B8G8R8A8_UNORM)
    {
        WIN_THROW_IF_FAILED(DirectX::Convert(
            *image.GetImage(0, 0, 0),
            DXGI_FORMAT::DXGI_FORMAT_B8G8R8A8_UNORM,
            DirectX::TEX_FILTER_DEFAULT,
            0.0,
            convert
        ));

        image = std::move(convert);
    }

    // Generate mips
    {
        WIN_THROW_IF_FAILED(DirectX::GenerateMipMaps(
            image.GetImages(), image.GetImageCount(), image.GetMetadata(),
            DirectX::TEX_FILTER_DEFAULT, 0, convert
        ));

        image = std::move(convert);
    }
    */

    return image;
}

}  // end namespace SD::RENDER
