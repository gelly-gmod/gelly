#ifndef TEXTURE_H
#define TEXTURE_H
#include <helpers/comptr.h>

#include <memory>

#include "device.h"
#include "image.h"

namespace gelly {
namespace renderer {

class Texture {
public:
	struct TextureCreateInfo {
		const std::shared_ptr<Device> device;
		const std::shared_ptr<Image> image;
		const D3D11_BIND_FLAG bindFlags;
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
