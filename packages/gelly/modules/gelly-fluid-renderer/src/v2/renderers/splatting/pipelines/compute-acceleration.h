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
inline auto CreateComputeAccelerationPipeline(const PipelineInfo &pipelineInfo)
	-> std::shared_ptr<ComputePipeline> {
	return ComputePipeline::CreateComputePipeline(
		ComputePipeline::ComputePipelineCreateInfo{
			.name = "Compute acceleration",
			.device = pipelineInfo.device,
			.computeShader =
				CS_FROM_GSC(ComputeAccelerationCS, pipelineInfo.device),
			.inputs =
				{InputBuffer{
					 .buffer =
						 pipelineInfo.internalBuffers->particleVelocities0,
					 .bindFlag = D3D11_BIND_SHADER_RESOURCE,
					 .slot = 0
				 },
				 InputBuffer{
					 .buffer =
						 pipelineInfo.internalBuffers->particleVelocities1,
					 .bindFlag = D3D11_BIND_SHADER_RESOURCE,
					 .slot = 1
				 },
				 InputBuffer{
					 .buffer =
						 pipelineInfo.internalBuffers->particleVelocities2,
					 .bindFlag = D3D11_BIND_SHADER_RESOURCE,
					 .slot = 2
				 },
				 InputBuffer{
					 .buffer =
						 pipelineInfo.internalBuffers->particleVelocities3,
					 .bindFlag = D3D11_BIND_SHADER_RESOURCE,
					 .slot = 3
				 },
				 InputBuffer{
					 .buffer =
						 pipelineInfo.internalBuffers->particleVelocities4,
					 .bindFlag = D3D11_BIND_SHADER_RESOURCE,
					 .slot = 4
				 },
				 InputBuffer{
					 .buffer =
						 pipelineInfo.internalBuffers->particleAccelerations,
					 .bindFlag = D3D11_BIND_UNORDERED_ACCESS,
					 .slot = 2
				 }},
			.outputs = {},
			.constantBuffers = {pipelineInfo.internalBuffers
									->computeAccelerationCBuffer.GetBuffer()},
			.threadGroupSize = {64, 1, 1},
			.repeatCount = 1,
		}
	);
}
}  // namespace splatting
}  // namespace renderer
}  // namespace gelly
#endif	// COMPUTE_ACCELERATION_H
