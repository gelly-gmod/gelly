#ifndef TEXTURE_H
#define TEXTURE_H
#include <helpers/comptr.h>

#include <memory>
#include <optional>

#include "device.h"
#include "image.h"

namespace gelly {
namespace renderer {

class Texture {
public:
	struct TextureCreateInfo {
		const std::shared_ptr<Device> device;
		const std::shared_ptr<Image> image;
		/**
		 * Will be overwritten with the format of the image if not provided.
		 * Useful for reinterpretation of the underlying image format.
		 */
		const std::optional<DXGI_FORMAT> format = std::nullopt;
		const UINT bindFlags;
	};

	explicit Texture(const TextureCreateInfo &createInfo);
	~Texture() = default;

	static auto CreateTexture(const TextureCreateInfo &&createInfo)
		-> std::shared_ptr<Texture>;

	auto GetTexture2D() -> ComPtr<ID3D11Texture2D>;
	auto GetShaderResourceView() -> ComPtr<ID3D11ShaderResourceView>;
	auto GetRenderTargetView() -> ComPtr<ID3D11RenderTargetView>;
	auto GetUnorderedAccessView() -> ComPtr<ID3D11UnorderedAccessView>;
	auto GetSamplerState() -> ComPtr<ID3D11SamplerState>;
	auto GetFormat() -> DXGI_FORMAT;

private:
	TextureCreateInfo createInfo;

	ComPtr<ID3D11ShaderResourceView> shaderResourceView;
	ComPtr<ID3D11RenderTargetView> renderTargetView;
	ComPtr<ID3D11UnorderedAccessView> unorderedAccessView;
	ComPtr<ID3D11SamplerState> samplerState;

	auto CreateShaderResourceView(const ComPtr<ID3D11Texture2D> &texture)
		-> ComPtr<ID3D11ShaderResourceView>;
	auto CreateRenderTargetView(const ComPtr<ID3D11Texture2D> &texture)
		-> ComPtr<ID3D11RenderTargetView>;
	auto CreateUnorderedAccessView(const ComPtr<ID3D11Texture2D> &texture)
		-> ComPtr<ID3D11UnorderedAccessView>;
	auto CreateSamplerState() -> ComPtr<ID3D11SamplerState>;
};

}  // namespace renderer
}  // namespace gelly

#endif	// TEXTURE_H
