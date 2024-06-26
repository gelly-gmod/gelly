#include "render-pass.h"

#include <helpers/throw-informative-exception.h>

#include <stdexcept>

namespace gelly {
namespace renderer {

RenderPass::RenderPass(const PassInfo &passInfo) : passInfo(passInfo) {
	depthStencilState = CreateDepthStencilState();
	rasterizerState = CreateRasterizerState();
	if (passInfo.blendState.has_value()) {
		blendState = CreateBlendState();
	}
}

auto RenderPass::Apply() -> void {
	auto *deviceContext = passInfo.device->GetRawDeviceContext().Get();

	deviceContext->OMSetDepthStencilState(depthStencilState.Get(), 0);
	deviceContext->RSSetState(rasterizerState.Get());
	auto viewport = CreateViewport();
	deviceContext->RSSetViewports(1, &viewport);
	if (passInfo.blendState.has_value()) {
		deviceContext->OMSetBlendState(blendState.Get(), nullptr, 0xFFFFFFFF);
	}
}

auto RenderPass::CreateDepthStencilState() -> ComPtr<ID3D11DepthStencilState> {
	auto *device = passInfo.device->GetRawDevice().Get();

	D3D11_DEPTH_STENCIL_DESC depthStencilDesc = {};
	depthStencilDesc.DepthEnable = passInfo.depthStencilState.depthTestEnabled;
	depthStencilDesc.DepthWriteMask =
		passInfo.depthStencilState.depthWriteEnabled
			? D3D11_DEPTH_WRITE_MASK_ALL
			: D3D11_DEPTH_WRITE_MASK_ZERO;
	depthStencilDesc.DepthFunc = passInfo.depthStencilState.depthComparisonFunc;
	depthStencilDesc.StencilEnable = false;

	ComPtr<ID3D11DepthStencilState> depthStencilState;
	const auto result = device->CreateDepthStencilState(
		&depthStencilDesc, depthStencilState.GetAddressOf()
	);

	GELLY_RENDERER_THROW_ON_FAIL(
		result, std::invalid_argument, "Failed to create depth stencil state"
	)

	return depthStencilState;
}

auto RenderPass::CreateRasterizerState() -> ComPtr<ID3D11RasterizerState> {
	auto *device = passInfo.device->GetRawDevice().Get();

	D3D11_RASTERIZER_DESC rasterizerDesc = {};
	rasterizerDesc.FillMode = passInfo.rasterizerState.fillMode;
	rasterizerDesc.CullMode = passInfo.rasterizerState.cullMode;
	rasterizerDesc.DepthBias = 0;
	rasterizerDesc.DepthBiasClamp = 0.0f;
	rasterizerDesc.SlopeScaledDepthBias = 0.0f;
	rasterizerDesc.DepthClipEnable = false;
	rasterizerDesc.ScissorEnable = false;
	rasterizerDesc.MultisampleEnable = false;
	rasterizerDesc.AntialiasedLineEnable = false;

	ComPtr<ID3D11RasterizerState> rasterizerState;
	const auto result = device->CreateRasterizerState(
		&rasterizerDesc, rasterizerState.GetAddressOf()
	);

	GELLY_RENDERER_THROW_ON_FAIL(
		result, std::invalid_argument, "Failed to create rasterizer state"
	)

	return rasterizerState;
}

auto RenderPass::CreateViewport() -> D3D11_VIEWPORT {
	D3D11_VIEWPORT viewport = {};
	viewport.TopLeftX = passInfo.viewportState.topLeftX;
	viewport.TopLeftY = passInfo.viewportState.topLeftY;
	viewport.Width = passInfo.viewportState.width;
	viewport.Height = passInfo.viewportState.height;
	viewport.MinDepth = passInfo.viewportState.minDepth;
	viewport.MaxDepth = passInfo.viewportState.maxDepth;

	return viewport;
}

auto RenderPass::CreateBlendState() -> ComPtr<ID3D11BlendState> {
	D3D11_BLEND_DESC blendDesc = {};
	blendDesc.AlphaToCoverageEnable = false;
	blendDesc.IndependentBlendEnable =
		passInfo.blendState->independentBlendEnable;

	std::memcpy(
		blendDesc.RenderTarget,
		passInfo.blendState->renderTarget,
		sizeof(D3D11_RENDER_TARGET_BLEND_DESC) * 8
	);

	ComPtr<ID3D11BlendState> blendState;
	const auto result = passInfo.device->GetRawDevice()->CreateBlendState(
		&blendDesc, blendState.GetAddressOf()
	);

	GELLY_RENDERER_THROW_ON_FAIL(
		result, std::invalid_argument, "Failed to create blend state"
	)

	return blendState;
}

}  // namespace renderer
}  // namespace gelly