#include "compute-pipeline.h"

namespace gelly {
namespace renderer {
ComputePipeline::ComputePipeline(const ComputePipelineCreateInfo &createInfo) :
	createInfo(createInfo) {}

auto ComputePipeline::CreateComputePipeline(
	const ComputePipelineCreateInfo &&createInfo
) -> std::shared_ptr<ComputePipeline> {
	return std::make_shared<ComputePipeline>(createInfo);
}

auto ComputePipeline::Dispatch(const WorkSize &workSize) -> void {
	const auto deviceContext = createInfo.device->GetRawDeviceContext();
	SetupReadOnlyInputs();
	SetupRWInputsAndOutputs();
	SetupConstantBuffers();

	deviceContext->Dispatch(workSize.width, workSize.height, workSize.depth);
}

auto ComputePipeline::SetupReadOnlyInputs() -> void {
	const auto deviceContext = createInfo.device->GetRawDeviceContext();
	for (const auto &input : createInfo.inputs) {
		if (const auto *texture = std::get_if<InputTexture>(&input);
			texture->bindFlag == D3D11_BIND_SHADER_RESOURCE) {
			deviceContext->CSSetShaderResources(
				texture->slot,
				1,
				texture->texture->GetShaderResourceView().GetAddressOf()
			);

			deviceContext->CSSetSamplers(
				texture->slot,
				1,
				texture->texture->GetSamplerState().GetAddressOf()
			);
		}

		if (const auto *buffer = std::get_if<InputBuffer>(&input);
			buffer->bindFlag == D3D11_BIND_SHADER_RESOURCE) {
			deviceContext->CSSetShaderResources(
				buffer->slot,
				1,
				buffer->buffer->GetShaderResourceView().GetAddressOf()
			);
		}
	}
}

auto ComputePipeline::SetupRWInputsAndOutputs() -> void {
	const auto deviceContext = createInfo.device->GetRawDeviceContext();
	std::vector<ID3D11UnorderedAccessView *> uavs;

	for (const auto &input : createInfo.inputs) {
		if (const auto *texture = std::get_if<InputTexture>(&input);
			texture->bindFlag == D3D11_BIND_UNORDERED_ACCESS) {
			uavs.push_back(texture->texture->GetUnorderedAccessView().Get());
		}

		if (const auto *buffer = std::get_if<InputBuffer>(&input);
			buffer->bindFlag == D3D11_BIND_UNORDERED_ACCESS) {
			uavs.push_back(buffer->buffer->GetUnorderedAccessView().Get());
		}
	}

	for (const auto &output : createInfo.outputs) {
		if (const auto *texture = std::get_if<OutputTexture>(&output);
			texture->bindFlag == D3D11_BIND_UNORDERED_ACCESS) {
			uavs.push_back(texture->texture->GetUnorderedAccessView().Get());
		}
	}

	deviceContext->CSSetUnorderedAccessViews(
		0, uavs.size(), uavs.data(), nullptr
	);
}

auto ComputePipeline::SetupConstantBuffers() -> void {
	const auto deviceContext = createInfo.device->GetRawDeviceContext();
	int bufferSlot = 0;
	for (const auto &buffer : createInfo.constantBuffers) {
		deviceContext->CSSetConstantBuffers(
			bufferSlot++, 1, buffer->GetRawBuffer().GetAddressOf()
		);
	}
}

}  // namespace renderer
}  // namespace gelly