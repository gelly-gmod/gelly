
#include "shared-image.h"

#include <stdexcept>

#include "helpers/throw-informative-exception.h"

namespace gelly {
namespace renderer {

SharedImage::SharedImage(const SharedImageCreateInfo &createInfo)
	: createInfo(createInfo) {
	texture2D = CreateTexture2D();
	textureDesc = GetTextureDesc(texture2D);
}

auto SharedImage::CreateSharedImage(const SharedImageCreateInfo &&createInfo)
	-> std::shared_ptr<SharedImage> {
	return std::make_shared<SharedImage>(createInfo);
}

auto SharedImage::GetTexture2D() -> ComPtr<ID3D11Texture2D> {
	return texture2D;
}

auto SharedImage::CreateTexture2D() -> ComPtr<ID3D11Texture2D> {
	ComPtr<ID3D11Resource> sharedResource;
	ComPtr<ID3D11Texture2D> sharedTexture;

	const auto sharedOpenResult =
		createInfo.device->GetRawDevice()->OpenSharedResource(
			createInfo.sharedHandle,
			__uuidof(ID3D11Resource),
			reinterpret_cast<void **>(sharedResource.GetAddressOf())
		);

	GELLY_RENDERER_THROW_ON_FAIL(
		sharedOpenResult,
		std::invalid_argument,
		"Failed to open shared resource"
	);

	const auto castResult = sharedResource.As(&sharedTexture);
	GELLY_RENDERER_THROW_ON_FAIL(
		castResult,
		std::runtime_error,
		"Failed to cast shared resource to texture"
	);

	return sharedTexture;
}

[[nodiscard]] auto SharedImage::GetTextureDesc(
	const ComPtr<ID3D11Texture2D> &texture2D
) const -> D3D11_TEXTURE2D_DESC {
	D3D11_TEXTURE2D_DESC desc;
	texture2D->GetDesc(&desc);
	return desc;
}

}  // namespace renderer
}  // namespace gelly