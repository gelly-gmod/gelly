#ifndef NATIVE_IMAGE_H
#define NATIVE_IMAGE_H

#include <memory>

#include "device.h"
#include "image.h"

namespace gelly {
namespace renderer {

class NativeImage : public Image {
public:
	struct NativeImageCreateInfo {
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
		const char *name;
	};

	explicit NativeImage(const NativeImageCreateInfo &createInfo);
	~NativeImage() override = default;

	static auto CreateNativeImage(const NativeImageCreateInfo &&createInfo)
		-> std::shared_ptr<Image>;

	auto GetTexture2D() -> ComPtr<ID3D11Texture2D> override;

	[[nodiscard]] auto GetWidth() const -> unsigned int override {
		return createInfo.width;
	}
	[[nodiscard]] auto GetHeight() const -> unsigned int override {
		return createInfo.height;
	}
	[[nodiscard]] auto GetFormat() const -> DXGI_FORMAT override {
		return createInfo.format;
	}
	[[nodiscard]] auto GetMipLevels() const -> UINT override {
		return createInfo.mipLevels;
	}

private:
	NativeImageCreateInfo createInfo;
	ComPtr<ID3D11Texture2D> texture2D;

	auto CreateTexture2D() -> ComPtr<ID3D11Texture2D>;
};

}  // namespace renderer
}  // namespace gelly

#endif	// NATIVE_IMAGE_H
