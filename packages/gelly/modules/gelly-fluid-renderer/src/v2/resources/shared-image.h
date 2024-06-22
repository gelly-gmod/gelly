#ifndef SHARED_IMAGE_H
#define SHARED_IMAGE_H
#include <memory>

#include "device.h"
#include "image.h"

namespace gelly {
namespace renderer {

class SharedImage : Image {
public:
	struct SharedImageCreateInfo {
		const std::shared_ptr<Device> device;
		HANDLE sharedHandle;
	};

	explicit SharedImage(const SharedImageCreateInfo &createInfo);
	~SharedImage() override = default;

	static auto CreateSharedImage(const SharedImageCreateInfo &&createInfo)
		-> std::shared_ptr<SharedImage>;

	auto GetTexture2D() -> ComPtr<ID3D11Texture2D> override;

	[[nodiscard]] auto GetWidth() const -> unsigned int override {
		return textureDesc.Width;
	}
	[[nodiscard]] auto GetHeight() const -> unsigned int override {
		return textureDesc.Height;
	}
	[[nodiscard]] auto GetFormat() const -> DXGI_FORMAT override {
		return textureDesc.Format;
	}
	[[nodiscard]] auto GetMipLevels() const -> UINT override {
		return textureDesc.MipLevels;
	}

private:
	SharedImageCreateInfo createInfo;
	ComPtr<ID3D11Texture2D> texture2D;
	D3D11_TEXTURE2D_DESC textureDesc;

	auto CreateTexture2D() -> ComPtr<ID3D11Texture2D>;
	[[nodiscard]] auto GetTextureDesc(const ComPtr<ID3D11Texture2D> &texture2D
	) const -> D3D11_TEXTURE2D_DESC;
};

}  // namespace renderer
}  // namespace gelly

#endif	// SHARED_IMAGE_H
