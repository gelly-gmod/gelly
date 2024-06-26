#include "pipeline.h"

#include <stdexcept>

#include "validate-pipeline.h"

template <typename... Ts>
struct overloaded : Ts... {
	using Ts::operator()...;
};
template <typename... Ts>
overloaded(Ts...) -> overloaded<Ts...>;

namespace gelly {
namespace renderer {
Pipeline::Pipeline(const PipelineCreateInfo &createInfo) :
	createInfo(createInfo),
	name(createInfo.name, createInfo.name + strlen(createInfo.name)) {
#ifdef GELLY_ENABLE_RENDERDOC_CAPTURES
	if (const auto validationErrors = ValidatePipeline(createInfo);
		validationErrors) {
		std::string errorMessage =
			GELLY_FUNCTION_NAME " Pipeline validation failed:";

		for (const auto &error : *validationErrors) {
			errorMessage += error.friendlyMessage + "\n";
		}

		throw std::runtime_error(errorMessage);
	}
#endif
}

auto Pipeline::CreatePipeline(const PipelineCreateInfo &&createInfo)
	-> std::shared_ptr<Pipeline> {
	return std::make_shared<Pipeline>(createInfo);
}

auto Pipeline::Run(const std::optional<int> vertexCount) -> void {
	auto *deviceContext = createInfo.device->GetRawDeviceContext().Get();
	createInfo.device->GetPerformanceMarker()->BeginEvent(name.c_str());

	// none of these are in any particular order
	if (createInfo.depthBuffer.has_value()) {
		deviceContext->ClearDepthStencilView(
			(*createInfo.depthBuffer)->GetDepthStencilView().Get(),
			D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
			1.0f,
			0
		);
	}

	SetupRenderPass();
	SetupInputAssembler();
	SetupOutputMerger();
	SetupShaderStages();

	deviceContext->Draw(vertexCount.value_or(createInfo.defaultVertexCount), 0);
	deviceContext->OMSetRenderTargets(0, nullptr, nullptr);
	deviceContext->ClearState();
	createInfo.device->GetPerformanceMarker()->EndEvent();
}

auto Pipeline::SetupRenderPass() -> void { createInfo.renderPass->Apply(); }

auto Pipeline::SetupInputAssembler() -> void {
	const auto deviceContext = createInfo.device->GetRawDeviceContext();
	std::vector<ID3D11Buffer *> vertexBuffers = {};
	std::vector<UINT> strides = {};
	std::vector<UINT> offsets = {};

	for (const auto &input : createInfo.inputs) {
		const auto *inputVertexBuffer = std::get_if<InputVertexBuffer>(&input);
		if (inputVertexBuffer) {
			vertexBuffers.push_back(
				inputVertexBuffer->vertexBuffer->GetRawBuffer().Get()
			);
			strides.push_back(inputVertexBuffer->vertexBuffer->GetBufferStride()
			);
			offsets.push_back(0);
		}
	}

	deviceContext->IASetVertexBuffers(
		0,
		vertexBuffers.size(),
		vertexBuffers.data(),
		strides.data(),
		offsets.data()
	);

	deviceContext->IASetInputLayout(
		createInfo.inputLayout->GetInputLayout().Get()
	);

	deviceContext->IASetPrimitiveTopology(createInfo.primitiveTopology);
}

auto Pipeline::SetupOutputMerger() -> void {
	auto *deviceContext = createInfo.device->GetRawDeviceContext().Get();
	int viewCount = 0;
	int uavCount = 0;

	ID3D11RenderTargetView *renderTargetViews[8] = {};
	ID3D11UnorderedAccessView *unorderedAccessViews[8] = {};

	for (const auto &output : createInfo.outputs) {
		const auto *outputTexture = std::get_if<OutputTexture>(&output);
		if (outputTexture) {
			renderTargetViews[outputTexture->slot] =
				outputTexture->texture->GetRenderTargetView().Get();
			viewCount++;

			if (outputTexture->clear) {
				deviceContext->ClearRenderTargetView(
					outputTexture->texture->GetRenderTargetView().Get(),
					outputTexture->clearColor
				);
			}
		}
	}

	// we also may have some input UAVs which do need to be bound at the same
	// time that outputs are typically bound
	for (const auto &input : createInfo.inputs) {
		const auto *inputTexture = std::get_if<InputTexture>(&input);
		if (inputTexture &&
			inputTexture->bindFlag == D3D11_BIND_UNORDERED_ACCESS) {
			unorderedAccessViews[inputTexture->slot] =
				inputTexture->texture->GetUnorderedAccessView().Get();
			uavCount++;
		}
	}

	const std::shared_ptr<DepthBuffer> depthBuffer =
		createInfo.depthBuffer.value_or(nullptr);

	ID3D11DepthStencilView *depthStencilView =
		depthBuffer ? depthBuffer->GetDepthStencilView().Get() : nullptr;

	if (uavCount > 0) {
		deviceContext->OMSetRenderTargetsAndUnorderedAccessViews(
			viewCount,
			renderTargetViews,
			depthStencilView,
			viewCount,	// start uavs after render targets to not overwrite them
			uavCount,
			unorderedAccessViews,
			nullptr
		);
	} else {
		deviceContext->OMSetRenderTargets(
			viewCount, renderTargetViews, depthStencilView
		);
	}
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

	for (const auto &input : createInfo.inputs) {
		const auto *inputTexture = std::get_if<InputTexture>(&input);
		if (inputTexture) {
			BindInputTexture(deviceContext, *inputTexture);
		}
	}

	for (const auto &input : createInfo.inputs) {
		const auto *inputBuffer = std::get_if<InputBuffer>(&input);
		if (inputBuffer) {
			BindInputBuffer(deviceContext, *inputBuffer);
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

			deviceContext->VSSetShaderResources(
				texture.slot,
				1,
				texture.texture->GetShaderResourceView().GetAddressOf()
			);
			deviceContext->VSSetSamplers(
				texture.slot,
				1,
				texture.texture->GetSamplerState().GetAddressOf()
			);
			break;
		default:
			break;
	}
}

auto Pipeline::BindInputBuffer(
	const ComPtr<ID3D11DeviceContext> &deviceContext, const InputBuffer &buffer
) -> void {
	switch (buffer.bindFlag) {
		case D3D11_BIND_SHADER_RESOURCE:
			deviceContext->VSSetShaderResources(
				buffer.slot,
				1,
				buffer.buffer->GetShaderResourceView().GetAddressOf()
			);
			break;
		case D3D11_BIND_UNORDERED_ACCESS:
			deviceContext->CSSetUnorderedAccessViews(
				buffer.slot,
				1,
				buffer.buffer->GetUnorderedAccessView().GetAddressOf(),
				nullptr
			);
			break;
		default:
			break;
	}
}

}  // namespace renderer
}  // namespace gelly