#include "depth-buffer.h"

#include <stdexcept>
#include <utility>

#include "helpers/throw-informative-exception.h"

namespace gelly {
namespace renderer {
DepthBuffer::DepthBuffer(const DepthBufferCreateInfo &createInfo) :
	createInfo(createInfo) {
	depthStencilView = CreateDepthStencilView();
}

auto DepthBuffer::CreateDepthBuffer(const DepthBufferCreateInfo &&createInfo)
	-> std::shared_ptr<DepthBuffer> {
	return std::make_shared<DepthBuffer>(createInfo);
}

auto DepthBuffer::GetDepthStencilView() -> ComPtr<ID3D11DepthStencilView> {
	return depthStencilView;
}

auto DepthBuffer::CreateDepthStencilView() -> ComPtr<ID3D11DepthStencilView> {
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc = {};
	depthStencilViewDesc.Format = createInfo.depthTexture->GetFormat();
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	ComPtr<ID3D11DepthStencilView> depthStencilView;
	auto depthTexture2D = createInfo.depthTexture->GetTexture2D();

	ComPtr<ID3D11Resource> depthTextureResource;
	const auto castResult = depthTexture2D.As(&depthTextureResource);
	GELLY_RENDERER_THROW_ON_FAIL(
		castResult,
		std::runtime_error,
		"Failed to cast depth tex to ID3D11Resource"
	);

	const auto result =
		createInfo.device->GetRawDevice()->CreateDepthStencilView(
			depthTextureResource.Get(),
			&depthStencilViewDesc,
			depthStencilView.GetAddressOf()
		);

	GELLY_RENDERER_THROW_ON_FAIL(
		result, std::invalid_argument, "Failed to create depth stencil view"
	);

	return depthStencilView;
}

}  // namespace renderer
}  // namespace gelly