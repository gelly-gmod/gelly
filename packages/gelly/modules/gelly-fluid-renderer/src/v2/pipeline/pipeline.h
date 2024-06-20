#ifndef PIPELINE_H
#define PIPELINE_H
#include <device.h>

#include <optional>
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
		const std::shared_ptr<Device> device;
		const std::shared_ptr<RenderPass> renderPass;
		const std::vector<Input> inputs;
		const std::vector<Output> outputs;
		const ShaderGroup shaderGroup;
		const std::optional<std::shared_ptr<DepthBuffer>> depthBuffer;
	};

	Pipeline(const PipelineCreateInfo &createInfo);
	~Pipeline() = default;

	auto Run(int vertexCount) -> void;

private:
	PipelineCreateInfo createInfo;

	auto SetupRenderPass() -> void;
	auto SetupInputAssembler() -> void;
	auto SetupOutputMerger() -> void;
	auto SetupShaderStages() -> void;

	auto BindInputTexture(
		const ComPtr<ID3D11DeviceContext> &deviceContext,
		const InputTexture &texture
	) -> void;
};

}  // namespace renderer
}  // namespace gelly

#endif	// PIPELINE_H
