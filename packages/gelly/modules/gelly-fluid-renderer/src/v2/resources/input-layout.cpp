#include "input-layout.h"

#include <helpers/throw-informative-exception.h>

#include <stdexcept>

namespace gelly {
namespace renderer {
InputLayout::InputLayout(const InputLayoutCreateInfo &createInfo)
	: createInfo(createInfo) {
	inputLayout = CreateInputLayout();
}

auto InputLayout::GetInputLayout() -> ComPtr<ID3D11InputLayout> {
	return inputLayout;
}

auto InputLayout::GetVertexShader() -> std::shared_ptr<VertexShader> {
	return createInfo.vertexShader;
}

auto InputLayout::CreateInputLayout() -> ComPtr<ID3D11InputLayout> {
	ComPtr<ID3D11InputLayout> inputLayout;
	const auto result = createInfo.device->GetRawDevice()->CreateInputLayout(
		createInfo.inputElementDescs,
		ARRAYSIZE(createInfo.inputElementDescs),
		createInfo.vertexShader->GetBlob(),
		createInfo.vertexShader->GetBlobSize(),
		inputLayout.GetAddressOf()
	);

	GELLY_RENDERER_THROW_ON_FAIL(
		result, std::invalid_argument, "Failed to create input layout"
	);

	return inputLayout;
}

}  // namespace renderer
}  // namespace gelly