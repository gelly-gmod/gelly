#ifndef GELLY_PIPELINE_H
#define GELLY_PIPELINE_H
#include <device.h>

#include <optional>
#include <string>
#include <vector>

#include "binding/input.h"
#include "binding/output.h"
#include "render-pass.h"

namespace gelly {
namespace renderer {

/**
 * A pipeline is essentially a declaration of the resources to be used in a
 * draw call.
 */
class Pipeline {
public:
	struct ShaderGroup {
		const std::shared_ptr<PixelShader> pixelShader;
		const std::shared_ptr<VertexShader> vertexShader;
		const std::optional<std::shared_ptr<GeometryShader>> geometryShader;

		/**
		 * Slot indices are computed by the order in which the constant buffers
		 * are inserted into the vector. (vector[0] == c0, vector[1] == c1, etc)
		 */
		const std::vector<std::shared_ptr<Buffer>> constantBuffers;
	};

	struct PipelineCreateInfo {
		/**
		 * Used for performance markers, this will appear in your rendering
		 * debugger of choice if supported.
		 */
		const char *name;
		const std::shared_ptr<Device> device;
		const std::shared_ptr<RenderPass> renderPass;
		const std::shared_ptr<InputLayout> inputLayout;
		const D3D11_PRIMITIVE_TOPOLOGY primitiveTopology;
		const std::vector<Input> inputs;
		const std::vector<Output> outputs;
		const ShaderGroup shaderGroup;
		const std::optional<std::shared_ptr<DepthBuffer>> depthBuffer;
		const int defaultVertexCount;
	};

	explicit Pipeline(const PipelineCreateInfo &createInfo);
	~Pipeline() = default;

	static auto CreatePipeline(const PipelineCreateInfo &&createInfo)
		-> std::shared_ptr<Pipeline>;

	auto Run(std::optional<int> vertexCount = std::nullopt) -> void;
	auto GetRenderPass() const -> std::shared_ptr<RenderPass> {
		return createInfo.renderPass;
	}

private:
	PipelineCreateInfo createInfo;
	std::wstring name;

	auto SetupRenderPass() -> void;
	auto SetupInputAssembler() -> void;
	auto SetupOutputMerger() -> void;
	auto SetupShaderStages() -> void;
	auto UpdateMipsForMipmappedTextures() -> void;

	static auto BindInputTexture(
		const ComPtr<ID3D11DeviceContext> &deviceContext,
		const InputTexture &texture
	) -> void;

	static auto BindInputBuffer(
		const ComPtr<ID3D11DeviceContext> &deviceContext,
		const InputBuffer &buffer
	) -> void;
};

}  // namespace renderer
}  // namespace gelly

#endif	// GELLY_PIPELINE_H
