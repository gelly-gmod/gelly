#include "pipeline.h"

template <typename... Ts>
struct overloaded : Ts... {
	using Ts::operator()...;
};
template <typename... Ts>
overloaded(Ts...) -> overloaded<Ts...>;

namespace gelly {
namespace renderer {
Pipeline::Pipeline(const PipelineCreateInfo &createInfo)
	: createInfo(createInfo) {}

auto Pipeline::Run(int vertexCount) -> void {
	auto *deviceContext = createInfo.device->GetRawDeviceContext().Get();

	// none of these are in any particular order
	SetupRenderPass();
	SetupInputAssembler();
	SetupOutputMerger();
	SetupShaderStages();

	deviceContext->Draw(vertexCount, 0);
	deviceContext->OMSetRenderTargets(0, nullptr, nullptr);
	deviceContext->ClearState();
}

auto Pipeline::SetupRenderPass() -> void { createInfo.renderPass->Apply(); }

auto Pipeline::SetupInputAssembler() -> void {
	auto deviceContext = createInfo.device->GetRawDeviceContext();
	std::vector<ID3D11Buffer *> vertexBuffers = {};
	std::vector<UINT> strides = {};
	std::vector<UINT> offsets = {};
	std::shared_ptr<InputLayout> vertexLayout = nullptr;

	for (const auto &input : createInfo.inputs) {
		const auto *inputVertexBuffer = std::get_if<InputVertexBuffer>(&input);
		if (inputVertexBuffer) {
			vertexBuffers.push_back(
				inputVertexBuffer->vertexBuffer->GetRawBuffer().Get()
			);
			strides.push_back(inputVertexBuffer->vertexBuffer->GetBufferStride()
			);
			offsets.push_back(0);
			vertexLayout = inputVertexBuffer->vertexLayout;
		}
	}

	deviceContext->IASetVertexBuffers(
		0,
		vertexBuffers.size(),
		vertexBuffers.data(),
		strides.data(),
		offsets.data()
	);

	deviceContext->IASetInputLayout(vertexLayout->GetInputLayout().Get());
}

auto Pipeline::SetupOutputMerger() -> void {
	auto *deviceContext = createInfo.device->GetRawDeviceContext().Get();
	int viewCount = 0;
	ID3D11RenderTargetView *renderTargetViews[8] = {};

	for (const auto &output : createInfo.outputs) {
		const auto *outputTexture = std::get_if<OutputTexture>(&output);
		if (outputTexture) {
			renderTargetViews[outputTexture->slot] =
				outputTexture->texture->GetRenderTargetView().Get();
			viewCount++;

			deviceContext->ClearRenderTargetView(
				outputTexture->texture->GetRenderTargetView().Get(),
				outputTexture->clearColor
			);
		}
	}

	const std::shared_ptr<DepthBuffer> depthBuffer =
		createInfo.depthBuffer.value_or(nullptr);

	ID3D11DepthStencilView *depthStencilView =
		depthBuffer ? depthBuffer->GetDepthStencilView().Get() : nullptr;

	deviceContext->OMSetRenderTargets(
		viewCount, renderTargetViews, depthStencilView
	);
}

auto Pipeline::SetupShaderStages() -> void {
	auto *deviceContext = createInfo.device->GetRawDeviceContext().Get();

	deviceContext->VSSetShader(
		createInfo.shaderGroup.vertexShader->GetRawShader().Get(), nullptr, 0
	);

	deviceContext->PSSetShader(
		createInfo.shaderGroup.pixelShader->GetRawShader().Get(), nullptr, 0
	);

	if (createInfo.shaderGroup.geometryShader) {
		deviceContext->GSSetShader(
			(*createInfo.shaderGroup.geometryShader)->GetRawShader().Get(),
			nullptr,
			0
		);
	}

	for (int slot = 0; slot < createInfo.shaderGroup.constantBuffers.size();
		 slot++) {
		deviceContext->VSSetConstantBuffers(
			slot,
			1,
			createInfo.shaderGroup.constantBuffers[slot]
				->GetRawBuffer()
				.GetAddressOf()
		);
		deviceContext->PSSetConstantBuffers(
			slot,
			1,
			createInfo.shaderGroup.constantBuffers[slot]
				->GetRawBuffer()
				.GetAddressOf()
		);

		if (createInfo.shaderGroup.geometryShader) {
			deviceContext->GSSetConstantBuffers(
				slot,
				1,
				createInfo.shaderGroup.constantBuffers[slot]
					->GetRawBuffer()
					.GetAddressOf()
			);
		}
	}
}

auto Pipeline::BindInputTexture(
	const ComPtr<ID3D11DeviceContext> &deviceContext,
	const InputTexture &texture
) -> void {
	switch (texture.bindFlag) {
		case D3D11_BIND_SHADER_RESOURCE:
			deviceContext->PSSetShaderResources(
				texture.slot,
				1,
				texture.texture->GetShaderResourceView().GetAddressOf()
			);
			deviceContext->PSSetSamplers(
				texture.slot,
				1,
				texture.texture->GetSamplerState().GetAddressOf()
			);
			break;
		case D3D11_BIND_RENDER_TARGET:
			deviceContext->OMSetRenderTargets(
				1,
				texture.texture->GetRenderTargetView().GetAddressOf(),
				nullptr
			);
			break;
		default:
			break;
	}
}

}  // namespace renderer
}  // namespace gelly