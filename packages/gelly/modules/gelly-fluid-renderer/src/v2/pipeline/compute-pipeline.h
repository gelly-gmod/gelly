#ifndef COMPUTE_PIPELINE_H
#define COMPUTE_PIPELINE_H
#include <memory>
#include <vector>

#include "binding/input.h"
#include "binding/output.h"
#include "resources/shader.h"

namespace gelly {
namespace renderer {

class ComputePipeline {
public:
	struct WorkSize {
		unsigned int width, height;
		unsigned int depth = 1;
	};

	/**
	 * Bear in mind, if you have an input which is a UAV and an output which is
	 * another UAV, they are both sent to a merged stage where the inputs are
	 * read first and then the outputs second.
	 *
	 * This is crucial if you have a UAV at input 1 with slot = 0, and a UAV at
	 * output 1 with slot = 0. Input 1 will be bound to register u0, and output
	 * 1 will be bound to register u1.
	 */
	struct ComputePipelineCreateInfo {
		const std::shared_ptr<Device> device;
		const char *name;
		const std::shared_ptr<ComputeShader> computeShader;
		const std::vector<Input> inputs;
		const std::vector<Output> outputs;
		const std::vector<std::shared_ptr<Buffer>> constantBuffers;
	};

	ComputePipeline(const ComputePipelineCreateInfo &createInfo);
	~ComputePipeline() = default;

	static auto CreateComputePipeline(
		const ComputePipelineCreateInfo &&createInfo
	) -> std::shared_ptr<ComputePipeline>;

	auto Dispatch(const WorkSize &workSize) -> void;

private:
	ComputePipelineCreateInfo createInfo;

	auto SetupReadOnlyInputs() -> void;
	auto SetupRWInputsAndOutputs() -> void;
	auto SetupConstantBuffers() -> void;
};

}  // namespace renderer
}  // namespace gelly

#endif	// COMPUTE_PIPELINE_H
