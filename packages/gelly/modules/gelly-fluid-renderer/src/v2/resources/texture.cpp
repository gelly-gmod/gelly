
#include "texture.h"

#include <stdexcept>

#include "helpers/parse-bind-flags.h"
#include "helpers/throw-informative-exception.h"

namespace gelly {
namespace renderer {

Texture::Texture(const TextureCreateInfo &createInfo) : createInfo(createInfo) {
	const auto texture2D = createInfo.image->GetTexture2D();
	const auto bindFlags = util::ParseBindFlags(this->createInfo.bindFlags);

	if (bindFlags.isRTVRequired) {
		renderTargetView = CreateRenderTargetView(texture2D);
	}

	if (bindFlags.isSRVRequired) {
		shaderResourceView = CreateShaderResourceView(texture2D);
	}

	if (bindFlags.isUAVRequired) {
		unorderedAccessView = CreateUnorderedAccessView(texture2D);
	}

	samplerState = CreateSamplerState();
}

auto Texture::CreateTexture(const TextureCreateInfo &&createInfo)
	-> std::shared_ptr<Texture> {
	return std::make_shared<Texture>(createInfo);
}

auto Texture::GetTexture2D() -> ComPtr<ID3D11Texture2D> {
	return createInfo.image->GetTexture2D();
}

auto Texture::GetShaderResourceView() -> ComPtr<ID3D11ShaderResourceView> {
	return shaderResourceView;
}

auto Texture::GetRenderTargetView() -> ComPtr<ID3D11RenderTargetView> {
	return renderTargetView;
}

auto Texture::GetUnorderedAccessView() -> ComPtr<ID3D11UnorderedAccessView> {
	return unorderedAccessView;
}

auto Texture::GetSamplerState() -> ComPtr<ID3D11SamplerState> {
	return samplerState;
}

auto Texture::GetFormat() -> DXGI_FORMAT {
	return createInfo.format.value_or(createInfo.image->GetFormat());
}

auto Texture::GetMipLevels() -> UINT {
	return createInfo.image->GetMipLevels();
}

auto Texture::CreateRenderTargetView(const ComPtr<ID3D11Texture2D> &texture)
	-> ComPtr<ID3D11RenderTargetView> {
	D3D11_RENDER_TARGET_VIEW_DESC desc = {};
	desc.Format = GetFormat();
	desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	desc.Texture2D.MipSlice = 0;

	ComPtr<ID3D11RenderTargetView> rtv;
	const auto result =
		createInfo.device->GetRawDevice()->CreateRenderTargetView(
			texture.Get(), &desc, &rtv
		);

	GELLY_RENDERER_THROW_ON_FAIL(
		result,
		std::invalid_argument,
		"Failed to create RenderTargetView with the supplied creation info."
	);

	return rtv;
}

auto Texture::CreateUnorderedAccessView(const ComPtr<ID3D11Texture2D> &texture)
	-> ComPtr<ID3D11UnorderedAccessView> {
	D3D11_UNORDERED_ACCESS_VIEW_DESC desc = {};
	desc.Format = GetFormat();
	desc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
	desc.Texture2D.MipSlice = 0;

	ComPtr<ID3D11UnorderedAccessView> uav;
	const auto result =
		createInfo.device->GetRawDevice()->CreateUnorderedAccessView(
			texture.Get(), &desc, &uav
		);

	GELLY_RENDERER_THROW_ON_FAIL(
		result,
		std::invalid_argument,
		"Failed to create UnorderedAccessView with the supplied creation info."
	);

	return uav;
}

auto Texture::CreateShaderResourceView(const ComPtr<ID3D11Texture2D> &texture)
	-> ComPtr<ID3D11ShaderResourceView> {
	D3D11_SHADER_RESOURCE_VIEW_DESC desc = {};
	desc.Format = GetFormat();
	desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	desc.Texture2D.MostDetailedMip = 0;
	desc.Texture2D.MipLevels = createInfo.image->GetMipLevels();

	ComPtr<ID3D11ShaderResourceView> srv;
	const auto result =
		createInfo.device->GetRawDevice()->CreateShaderResourceView(
			texture.Get(), &desc, &srv
		);

	GELLY_RENDERER_THROW_ON_FAIL(
		result,
		std::invalid_argument,
		"Failed to create ShaderResourceView with the supplied creation "
		"info."
	);

	return srv;
}

auto Texture::CreateSamplerState() -> ComPtr<ID3D11SamplerState> {
	D3D11_SAMPLER_DESC desc = {};
	desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	desc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	desc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	desc.MinLOD = 0;
	desc.MaxLOD = D3D11_FLOAT32_MAX;
	desc.MaxAnisotropy = 1;
	desc.BorderColor[0] = 0;
	desc.BorderColor[1] = 0;
	desc.BorderColor[2] = 0;
	desc.BorderColor[3] = 0;

	ComPtr<ID3D11SamplerState> sampler;

	const auto result =
		createInfo.device->GetRawDevice()->CreateSamplerState(&desc, &sampler);

	GELLY_RENDERER_THROW_ON_FAIL(
		result,
		std::invalid_argument,
		"Failed to create a texture sampler with the supplied creation "
		"info."
	);

	return sampler;
}

}  // namespace renderer
}  // namespace gelly