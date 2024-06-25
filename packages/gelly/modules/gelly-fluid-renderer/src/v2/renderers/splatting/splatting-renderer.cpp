#include "splatting-renderer.h"

#include "pipelines/depth-filtering.h"
#include "pipelines/ellipsoid-splatting.h"
#include "pipelines/normal-estimation.h"

namespace gelly {
namespace renderer {
namespace splatting {
SplattingRenderer::SplattingRenderer(
	const SplattingRendererCreateInfo &createInfo
) :
	createInfo(createInfo), pipelineInfo(CreatePipelineInfo()) {
	CreatePipelines();
	LinkBuffersToSimData();
}

auto SplattingRenderer::Create(const SplattingRendererCreateInfo &&createInfo)
	-> std::shared_ptr<SplattingRenderer> {
	return std::make_shared<SplattingRenderer>(createInfo);
}

auto SplattingRenderer::Render() const -> void {
	ellipsoidSplatting->Run(createInfo.simData->GetActiveParticles());
	depthFilteringA->Run();
	depthFilteringB->Run();
	normalEstimation->Run();
}

auto SplattingRenderer::UpdateSettings(const Settings &settings) -> void {
	this->settings = settings;
}

auto SplattingRenderer::UpdateFrameParams(
	const cbuffer::FluidRenderCBufferData &data
) const -> void {
	pipelineInfo.internalBuffers->fluidRenderCBuffer.UpdateBuffer(data);
}

auto SplattingRenderer::CreatePipelines() -> void {
	ellipsoidSplatting = CreateEllipsoidSplattingPipeline(pipelineInfo);
	depthFilteringA = CreateDepthFilteringPipeline(
		pipelineInfo,
		pipelineInfo.internalTextures->unfilteredEllipsoidDepth,
		pipelineInfo.outputTextures->ellipsoidDepth
	);
	depthFilteringB = CreateDepthFilteringPipeline(
		pipelineInfo,
		pipelineInfo.outputTextures->ellipsoidDepth,
		pipelineInfo.internalTextures->unfilteredEllipsoidDepth
	);
	normalEstimation = CreateNormalEstimationPipeline(pipelineInfo);
}

auto SplattingRenderer::CreatePipelineInfo() const -> PipelineInfo {
	return {
		.device = createInfo.device,
		.internalTextures = std::make_shared<InternalTextures>(
			createInfo.device, createInfo.width, createInfo.height
		),
		.outputTextures = std::make_shared<OutputTextures>(
			createInfo.device, createInfo.inputSharedHandles
		),
		.internalBuffers = std::make_shared<InternalBuffers>(
			createInfo.device, createInfo.maxParticles
		),
		.width = createInfo.width,
		.height = createInfo.height
	};
}

auto SplattingRenderer::LinkBuffersToSimData() const -> void {
	const auto simData = createInfo.simData;

	simData->LinkBuffer(
		SimBufferType::POSITION,
		pipelineInfo.internalBuffers->particlePositions->GetRawBuffer().Get()
	);

	simData->LinkBuffer(
		SimBufferType::ANISOTROPY_Q1,
		pipelineInfo.internalBuffers->anisotropyQ1->GetRawBuffer().Get()
	);

	simData->LinkBuffer(
		SimBufferType::ANISOTROPY_Q2,
		pipelineInfo.internalBuffers->anisotropyQ2->GetRawBuffer().Get()
	);

	simData->LinkBuffer(
		SimBufferType::ANISOTROPY_Q3,
		pipelineInfo.internalBuffers->anisotropyQ3->GetRawBuffer().Get()
	);

	simData->LinkBuffer(
		SimBufferType::FOAM_POSITION,
		pipelineInfo.internalBuffers->foamPositions->GetRawBuffer().Get()
	);

	simData->LinkBuffer(
		SimBufferType::FOAM_VELOCITY,
		pipelineInfo.internalBuffers->foamVelocities->GetRawBuffer().Get()
	);
}

}  // namespace splatting
}  // namespace renderer
}  // namespace gelly