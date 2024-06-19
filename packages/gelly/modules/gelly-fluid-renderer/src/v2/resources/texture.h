#ifndef TEXTURE_H
#define TEXTURE_H
#include <helpers/comptr.h>

#include <memory>

#include "device.h"

namespace gelly {
namespace renderer {

class Texture {
public:
	struct TextureCreateInfo {
		const std::shared_ptr<Device> device;
		const unsigned int width;
		const unsigned int height;
		const DXGI_FORMAT format;
		const D3D11_USAGE usage;
		const UINT bindFlags;
		const UINT cpuAccessFlags = 0;
		const UINT miscFlags = 0;
		const UINT arraySize = 1;
		const UINT mipLevels = 1;
	};

	Texture(const TextureCreateInfo &createInfo);
	~Texture() = default;

	auto GetTexture2D() -> ComPtr<ID3D11Texture2D>;
	auto GetShaderResourceView() -> ComPtr<ID3D11ShaderResourceView>;
	auto GetRenderTargetView() -> ComPtr<ID3D11RenderTargetView>;
	auto GetUnorderedAccessView() -> ComPtr<ID3D11UnorderedAccessView>;

private:
	TextureCreateInfo createInfo;

	ComPtr<ID3D11Texture2D> texture2D;
	ComPtr<ID3D11ShaderResourceView> shaderResourceView;
	ComPtr<ID3D11RenderTargetView> renderTargetView;
	ComPtr<ID3D11UnorderedAccessView> unorderedAccessView;

	auto CreateTexture2D() -> ComPtr<ID3D11Texture2D>;
	auto CreateShaderResourceView(const ComPtr<ID3D11Texture2D> &texture)
		-> ComPtr<ID3D11ShaderResourceView>;
	auto CreateRenderTargetView(const ComPtr<ID3D11Texture2D> &texture)
		-> ComPtr<ID3D11RenderTargetView>;
	auto CreateUnorderedAccessView(const ComPtr<ID3D11Texture2D> &texture)
		-> ComPtr<ID3D11UnorderedAccessView>;

	auto IsRTVRequired() -> bool;
	auto IsSRVRequired() -> bool;
	auto IsUAVRequired() -> bool;
};

}  // namespace renderer
}  // namespace gelly

#endif	// TEXTURE_H
