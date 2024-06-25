#include "input-layout.h"

#include <helpers/throw-informative-exception.h>

#include <stdexcept>
#include <string>

namespace gelly {
namespace renderer {
InputLayout::InputLayout(const InputLayoutCreateInfo &createInfo) :
	createInfo(createInfo) {
	inputLayout = CreateInputLayout();
}

auto InputLayout::GetInputLayout() -> ComPtr<ID3D11InputLayout> {
	return inputLayout;
}

auto InputLayout::GetVertexShader() -> std::shared_ptr<VertexShader> {
	return createInfo.vertexShader;
}

auto InputLayout::CreateInputLayout() -> ComPtr<ID3D11InputLayout> {
	ComPtr<ID3D11InputLayout> layout;
	const auto layoutResult =
		createInfo.device->GetRawDevice()->CreateInputLayout(
			createInfo.inputElements.data(),
			static_cast<UINT>(createInfo.inputElements.size()),
			createInfo.vertexShader->GetBlob(),
			createInfo.vertexShader->GetBlobSize(),
			layout.GetAddressOf()
		);

	GELLY_RENDERER_THROW_ON_FAIL(
		layoutResult, std::invalid_argument, "Failed to create input layout"
	);

	return layout;
}

}  // namespace renderer
}  // namespace gelly