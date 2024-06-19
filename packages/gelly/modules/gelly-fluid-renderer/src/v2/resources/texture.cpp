
#include "texture.h"

namespace gelly {
namespace renderer {

Texture::Texture(const TextureCreateInfo &createInfo)
	: createInfo(std::move(createInfo)) {
	texture2D = CreateTexture2D();

	if (IsRTVRequired()) {
		renderTargetView = CreateRenderTargetView(texture2D);
	}

	if (IsSRVRequired()) {
		shaderResourceView = CreateShaderResourceView(texture2D);
	}

	if (IsUAVRequired()) {
		unorderedAccessView = CreateUnorderedAccessView(texture2D);
	}
}
}  // namespace renderer
}  // namespace gelly