#ifndef COMPUTE_ACCELERATION_H
#define COMPUTE_ACCELERATION_H
#include <memory>

#include "ComputeAccelerationCS.h"
#include "helpers/create-gsc-shader.h"
#include "pipeline-info.h"
#include "pipeline/compute-pipeline.h"

namespace gelly {
namespace renderer {
namespace splatting {
inline auto CreateComputeAccelerationPipeline(
	const PipelineInfo &pipelineInfo, const size_t frameIndex
) -> std::shared_ptr<ComputePipeline> {
	return ComputePipeline::CreateComputePipeline(
		ComputePipeline::ComputePipelineCreateInfo{
			.name = "Compute acceleration",
			.device = pipelineInfo.device,
			.computeShader =
				CS_FROM_GSC(ComputeAccelerationCS, pipelineInfo.device),
			.inputs =
				{InputBuffer{
					 .buffer = pipelineInfo.internalBuffers[frameIndex]
								   ->particleVelocities0,
					 .bindFlag = D3D11_BIND_SHADER_RESOURCE,
					 .slot = 0
				 },
				 InputBuffer{
					 .buffer = pipelineInfo.internalBuffers[frameIndex]
								   ->particleVelocities1,
					 .bindFlag = D3D11_BIND_SHADER_RESOURCE,
					 .slot = 1
				 },
				 InputBuffer{
					 .buffer = pipelineInfo.internalBuffers[frameIndex]
								   ->particleAccelerations,
					 .bindFlag = D3D11_BIND_UNORDERED_ACCESS,
					 .slot = 2
				 }},
			.outputs = {},
			.constantBuffers = {},
			.threadGroupSize = {64, 1, 1},
			.repeatCount = 1,
		}
	);
}
}  // namespace splatting
}  // namespace renderer
}  // namespace gelly
#endif	// COMPUTE_ACCELERATION_H
