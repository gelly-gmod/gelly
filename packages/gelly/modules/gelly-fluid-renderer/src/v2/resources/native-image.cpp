#include "native-image.h"

#include <stdexcept>

#include "helpers/throw-informative-exception.h"

namespace gelly {
namespace renderer {
NativeImage::NativeImage(const NativeImageCreateInfo &createInfo) :
	createInfo(createInfo) {
	texture2D = CreateTexture2D();
}

auto NativeImage::CreateNativeImage(const NativeImageCreateInfo &&createInfo)
	-> std::shared_ptr<Image> {
	return std::make_shared<NativeImage>(createInfo);
}

auto NativeImage::GetTexture2D() -> ComPtr<ID3D11Texture2D> {
	return texture2D;
}

auto NativeImage::CreateTexture2D() -> ComPtr<ID3D11Texture2D> {
	D3D11_TEXTURE2D_DESC desc = {};
	desc.Width = createInfo.width;
	desc.Height = createInfo.height;
	desc.MipLevels = createInfo.mipLevels;
	desc.ArraySize = createInfo.arraySize;
	desc.Format = createInfo.format;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = createInfo.usage;
	desc.BindFlags = createInfo.bindFlags;
	desc.CPUAccessFlags = createInfo.cpuAccessFlags;
	desc.MiscFlags = createInfo.miscFlags;

	if (createInfo.mipLevels > 1) {
		desc.MiscFlags |= D3D11_RESOURCE_MISC_GENERATE_MIPS;
	}

	ComPtr<ID3D11Texture2D> texture;
	const auto result = createInfo.device->GetRawDevice()->CreateTexture2D(
		&desc, nullptr, &texture
	);

	GELLY_RENDERER_THROW_ON_FAIL(
		result,
		std::invalid_argument,
		"Failed to create Texture2D with the supplied creation info."
	);

	// set the debug name
	texture->SetPrivateData(
		WKPDID_D3DDebugObjectName,
		static_cast<UINT>(strlen(createInfo.name)),
		createInfo.name
	);

	return texture;
}

}  // namespace renderer
}  // namespace gelly