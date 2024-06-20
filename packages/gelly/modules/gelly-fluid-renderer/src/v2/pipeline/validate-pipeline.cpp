#include "validate-pipeline.h"

#include <algorithm>

#include "helpers/parse-bind-flags.h"

namespace gelly {
namespace renderer {
template <typename... Ts>
struct overloaded : Ts... {
	using Ts::operator()...;
};
template <typename... Ts>
overloaded(Ts...) -> overloaded<Ts...>;

auto ValidateInputTexture(const InputTexture &texture)
	-> std::optional<PipelineValidationError> {
	if (texture.slot >= D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT) {
		return PipelineValidationError{"Input texture slot is out of range."};
	}

	const auto primaryBindType = util::FindPrimaryBindType(texture.bindFlag);
	if (!primaryBindType) {
		return PipelineValidationError{
			"Input texture has multiple bind flags. It should only have one "
			"primary bind flag."
		};
	}

	const auto bindType = *primaryBindType;
	if (bindType != util::PrimaryBindType::SRV &&
		bindType != util::PrimaryBindType::UAV) {
		return PipelineValidationError{
			"Input texture has an invalid bind flag. It should be either SRV "
			"or UAV."
		};
	}

	return std::nullopt;
}

auto ValidateInputBuffer(const InputBuffer &buffer)
	-> std::optional<PipelineValidationError> {
	if (buffer.slot >= D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT) {
		return PipelineValidationError{"Input buffer slot is out of range."};
	}

	const auto primaryBindType = util::FindPrimaryBindType(buffer.bindFlag);
	if (!primaryBindType) {
		return PipelineValidationError{
			"Input buffer has multiple bind flags. It should only have one "
			"primary bind flag."
		};
	}

	const auto bindType = *primaryBindType;
	if (bindType != util::PrimaryBindType::SRV &&
		bindType != util::PrimaryBindType::UAV) {
		return PipelineValidationError{
			"Input buffer has an invalid bind flag. It should be either SRV "
			"or UAV."
		};
	}

	return std::nullopt;
}

auto ValidateOutputTexture(const OutputTexture &texture)
	-> std::optional<PipelineValidationError> {
	if (texture.slot >= D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT) {
		return PipelineValidationError{"Output texture slot is out of range."};
	}

	const auto primaryBindType = util::FindPrimaryBindType(texture.bindFlag);
	if (!primaryBindType) {
		return PipelineValidationError{
			"Output texture has multiple bind flags. It should only have one "
			"primary bind flag."
		};
	}

	const auto bindType = *primaryBindType;
	if (bindType != util::PrimaryBindType::RTV) {
		return PipelineValidationError{
			"Output texture has an invalid bind flag. It should be RTV."
		};
	}

	return std::nullopt;
}

auto ValidateInputVertexBuffer(const InputVertexBuffer &vertexBuffer)
	-> std::optional<PipelineValidationError> {
	if (vertexBuffer.slot >= D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT) {
		return PipelineValidationError{
			"Input vertex buffer slot is out of range."
		};
	}

	const auto underlyingBufferBindFlags =
		vertexBuffer.vertexBuffer->GetBufferBindFlags();

	const auto primaryBindType =
		util::FindPrimaryBindType(underlyingBufferBindFlags);

	if (!primaryBindType) {
		return PipelineValidationError{
			"Input vertex buffer has multiple bind flags. It should only have "
			"one primary bind flag."
		};
	}

	if (*primaryBindType != util::PrimaryBindType::VERTEX_BUFFER) {
		return PipelineValidationError{
			"Input vertex buffer has an invalid bind flag. It should be "
			"VERTEX_BUFFER."
		};
	}

	return std::nullopt;
}

auto ValidateConstantBuffer(const std::shared_ptr<Buffer> constantBuffer) {
	const auto underlyingBufferBindFlags = constantBuffer->GetBufferBindFlags();

	const auto primaryBindType =
		util::FindPrimaryBindType(underlyingBufferBindFlags);

	if (!primaryBindType) {
		return PipelineValidationError{
			"Constant buffer has multiple bind flags. It should only have one "
			"primary bind flag."
		};
	}

	if (*primaryBindType != util::PrimaryBindType::CONSTANT_BUFFER) {
		return PipelineValidationError{
			"Constant buffer has an invalid bind flag. It should be "
			"CONSTANT_BUFFER."
		};
	}
}

auto ValidatePipeline(const Pipeline::PipelineCreateInfo &createInfo)
	-> std::optional<std::vector<PipelineValidationError>> {
	std::vector<PipelineValidationError> errors;

	std::for_each(
		createInfo.inputs.begin(),
		createInfo.inputs.end(),
		[&errors](const auto &input) {
			const auto error = std::visit(
				overloaded{
					[](const InputTexture &texture) {
						return ValidateInputTexture(texture);
					},
					[](const InputBuffer &buffer) {
						return ValidateInputBuffer(buffer);
					},
					[](const InputVertexBuffer &vertexBuffer) {
						return ValidateInputVertexBuffer(vertexBuffer);
					},
				},
				input
			);

			if (error) {
				errors.push_back(*error);
			}
		}
	);

	std::for_each(
		createInfo.outputs.begin(),
		createInfo.outputs.end(),
		[&errors](const auto &output) {
			const auto error = std::visit(
				overloaded{
					[](const OutputTexture &texture) {
						return ValidateOutputTexture(texture);
					},
				},
				output
			);

			if (error) {
				errors.push_back(*error);
			}
		}
	);

	std::for_each(
		createInfo.shaderGroup.constantBuffers.begin(),
		createInfo.shaderGroup.constantBuffers.end(),
		[&errors](const auto *buffer) {
			const auto error = ValidateConstantBuffer(buffer);
			if (error) {
				errors.push_back(*error);
			}
		}
	);

	if (errors.empty()) {
		return std::nullopt;
	}

	return {errors};
}

}  // namespace renderer
}  // namespace gelly