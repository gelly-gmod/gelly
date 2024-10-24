#include "splatting-renderer.h"

#include "pipelines/albedo-downsampling.h"
#include "pipelines/compute-acceleration.h"
#include "pipelines/ellipsoid-splatting.h"
#include "pipelines/normal-estimation.h"
#include "pipelines/spray-splatting.h"
#include "pipelines/surface-filtering.h"
#include "pipelines/thickness-splatting.h"
#ifdef GELLY_ENABLE_RENDERDOC_CAPTURES
#include "reload-shaders.h"
#endif

namespace gelly {
namespace renderer {
namespace splatting {

AbsorptionModifier::AbsorptionModifier(
	const AbsorptionModifierCreateInfo &createInfo
) :
	createInfo(createInfo), bufferView() {}

auto AbsorptionModifier::StartModifying() -> void {
	bufferView = std::make_shared<BufferView>(BufferView::BufferViewCreateInfo{
		.device = createInfo.device,
		.buffer = createInfo.absorptionBuffer,
		.mapType = D3D11_MAP_WRITE_NO_OVERWRITE
	});
}

auto AbsorptionModifier::ModifyAbsorption(int particleIndex, float3 absorption)
	-> void {
	bufferView->Write(particleIndex, absorption);
}

auto AbsorptionModifier::EndModifying() -> void { bufferView.reset(); }

SplattingRenderer::SplattingRenderer(
	const SplattingRendererCreateInfo &createInfo
) :
	createInfo(createInfo),
	pipelineInfo(CreatePipelineInfo()),
	query(CreateQuery()),
	durations(
		{.computeAcceleration = createInfo.device,
		 .ellipsoidSplatting = createInfo.device,
		 .thicknessSplatting = createInfo.device,
		 .albedoDownsampling = createInfo.device,
		 .surfaceFiltering = createInfo.device,
		 .rawNormalEstimation = createInfo.device}
	) {
	CreatePipelines();
	LinkBuffersToSimData();
	absorptionModifier = CreateAbsorptionModifier(
		pipelineInfo.internalBuffers->particleAbsorptions
	);

#ifdef GELLY_ENABLE_RENDERDOC_CAPTURES
	renderDoc = InstantiateRenderDoc();
#endif
}

auto SplattingRenderer::Create(const SplattingRendererCreateInfo &&createInfo)
	-> std::shared_ptr<SplattingRenderer> {
	return std::make_shared<SplattingRenderer>(createInfo);
}

auto SplattingRenderer::Render() -> void {
#ifdef GELLY_ENABLE_RENDERDOC_CAPTURES
	if (renderDoc) {
		renderDoc->StartFrameCapture(
			createInfo.device->GetRawDevice().Get(), nullptr
		);
	}
#endif
	if (settings.enableGPUTiming) {
		durations.computeAcceleration.Start();
	}
	computeAcceleration->Dispatch(
		{static_cast<unsigned int>(createInfo.simData->GetActiveParticles()),
		 1,
		 1}
	);
	if (settings.enableGPUTiming) {
		durations.computeAcceleration.End();
	}

	if (settings.enableGPUTiming) {
		durations.spraySplatting.Start();
	}

	SetFrameResolution(
		spraySplatting->GetRenderPass()->GetScaledWidth(),
		spraySplatting->GetRenderPass()->GetScaledHeight()
	);

	spraySplatting->Run(createInfo.simData->GetActiveFoamParticles());

	if (settings.enableGPUTiming) {
		durations.spraySplatting.End();
	}

	if (settings.enableGPUTiming) {
		durations.ellipsoidSplatting.Start();
	}
	SetFrameResolution(
		ellipsoidSplatting->GetRenderPass()->GetScaledWidth(),
		ellipsoidSplatting->GetRenderPass()->GetScaledHeight()
	);
	ellipsoidSplatting->Run(createInfo.simData->GetActiveParticles());
	if (settings.enableGPUTiming) {
		durations.ellipsoidSplatting.End();
	}

	SetFrameResolution(
		thicknessSplatting->GetRenderPass()->GetScaledWidth(),
		thicknessSplatting->GetRenderPass()->GetScaledHeight()
	);

	if (settings.enableGPUTiming) {
		durations.thicknessSplatting.Start();
	}
	thicknessSplatting->Run(createInfo.simData->GetActiveParticles());
	if (settings.enableGPUTiming) {
		durations.thicknessSplatting.End();
	}

	SetFrameResolution(
		albedoDownsampling->GetRenderPass()->GetScaledWidth(),
		albedoDownsampling->GetRenderPass()->GetScaledHeight()
	);
	if (settings.enableGPUTiming) {
		durations.albedoDownsampling.Start();
	}
	albedoDownsampling->Run();
	if (settings.enableGPUTiming) {
		durations.albedoDownsampling.End();
	}
	SetFrameResolution(
		surfaceFilteringA->GetRenderPass()->GetScaledWidth(),
		surfaceFilteringA->GetRenderPass()->GetScaledHeight()
	);
	if (settings.enableGPUTiming) {
		durations.rawNormalEstimation.Start();
	}
	SetFrameResolution(
		rawNormalEstimation->GetRenderPass()->GetScaledWidth(),
		rawNormalEstimation->GetRenderPass()->GetScaledHeight()
	);
	rawNormalEstimation->Run();
	if (settings.enableGPUTiming) {
		durations.rawNormalEstimation.End();
	}

	if (settings.enableGPUTiming) {
		durations.surfaceFiltering.Start();
	}
	RunSurfaceFilteringPipeline(settings.filterIterations);
	if (settings.enableGPUTiming) {
		durations.surfaceFiltering.End();
	}
#ifdef GELLY_ENABLE_RENDERDOC_CAPTURES
	if (renderDoc) {
		renderDoc->EndFrameCapture(
			createInfo.device->GetRawDevice().Get(), nullptr
		);
	}
#endif

	if (settings.enableGPUSynchronization) {
		createInfo.device->GetRawDeviceContext()->End(query.Get());

		// busy wait until the query is done
		while (createInfo.device->GetRawDeviceContext()->GetData(
				   query.Get(), nullptr, 0, 0
			   ) == S_FALSE) {
			Sleep(0);
		}
	}

	if (settings.enableGPUTiming) {
		latestTimings.computeAcceleration =
			durations.computeAcceleration.GetDuration();
		latestTimings.ellipsoidSplatting =
			durations.ellipsoidSplatting.GetDuration();
		latestTimings.thicknessSplatting =
			durations.thicknessSplatting.GetDuration();
		latestTimings.albedoDownsampling =
			durations.albedoDownsampling.GetDuration();
		latestTimings.surfaceFiltering =
			durations.surfaceFiltering.GetDuration();
		latestTimings.rawNormalEstimation =
			durations.rawNormalEstimation.GetDuration();

		latestTimings.isDisjoint = durations.computeAcceleration.IsDisjoint() ||
								   durations.ellipsoidSplatting.IsDisjoint() ||
								   durations.thicknessSplatting.IsDisjoint() ||
								   durations.albedoDownsampling.IsDisjoint() ||
								   durations.surfaceFiltering.IsDisjoint() ||
								   durations.rawNormalEstimation.IsDisjoint();
	}
}

auto SplattingRenderer::GetAbsorptionModifier() const
	-> std::shared_ptr<AbsorptionModifier> {
	return absorptionModifier;
}

auto SplattingRenderer::GetSettings() const -> Settings { return settings; }

auto SplattingRenderer::UpdateSettings(const Settings &settings) -> void {
	this->settings = settings;
}

auto SplattingRenderer::FetchTimings() -> Timings { return latestTimings; }

auto SplattingRenderer::UpdateFrameParams(cbuffer::FluidRenderCBufferData &data)
	-> void {
	pipelineInfo.internalBuffers->fluidRenderCBuffer.UpdateBuffer(data);
	std::memcpy(
		&frameParamCopy, &data, sizeof(cbuffer::FluidRenderCBufferData)
	);
}

auto SplattingRenderer::SetFrameResolution(float width, float height) -> void {
	frameParamCopy.g_ViewportWidth = width;
	frameParamCopy.g_ViewportHeight = height;
	frameParamCopy.g_InvViewport.x = 1.f / width;
	frameParamCopy.g_InvViewport.y = 1.f / height;

	pipelineInfo.internalBuffers->fluidRenderCBuffer.UpdateBuffer(frameParamCopy
	);
}

auto SplattingRenderer::CreatePipelines() -> void {
	computeAcceleration = CreateComputeAccelerationPipeline(pipelineInfo);
	spraySplatting =
		CreateSpraySplattingPipeline(pipelineInfo, createInfo.scale);
	ellipsoidSplatting =
		CreateEllipsoidSplattingPipeline(pipelineInfo, createInfo.scale);
	thicknessSplatting =
		CreateThicknessSplattingPipeline(pipelineInfo, createInfo.scale);
	albedoDownsampling =
		CreateAlbedoDownsamplingPipeline(pipelineInfo, ALBEDO_OUTPUT_SCALE);

	surfaceFilteringA = CreateSurfaceFilteringPipeline(
		pipelineInfo,
		pipelineInfo.internalTextures->unfilteredNormals,
		pipelineInfo.outputTextures->normals,
		createInfo.scale
	);
	surfaceFilteringB = CreateSurfaceFilteringPipeline(
		pipelineInfo,
		pipelineInfo.outputTextures->normals,
		pipelineInfo.internalTextures->unfilteredNormals,
		createInfo.scale
	);

	rawNormalEstimation = CreateNormalEstimationPipeline(
		pipelineInfo,
		pipelineInfo.outputTextures->ellipsoidDepth,
		pipelineInfo.internalTextures->unfilteredNormals,
		createInfo.scale
	);
}

auto SplattingRenderer::UpdateTextureRegistry(
	const InputSharedHandles &inputSharedHandles,
	float width,
	float height,
	float scale
) -> void {
	createInfo.width = width;
	createInfo.height = height;
	createInfo.inputSharedHandles = inputSharedHandles;
	createInfo.scale = scale;

	pipelineInfo.width = width;
	pipelineInfo.height = height;

	pipelineInfo.internalTextures = std::make_shared<InternalTextures>(
		createInfo.device,
		createInfo.width,
		createInfo.height,
		createInfo.scale,
		ALBEDO_OUTPUT_SCALE
	);

	pipelineInfo.outputTextures = std::make_shared<OutputTextures>(
		createInfo.device, createInfo.inputSharedHandles
	);

	// Re-initialize all pipelines
	CreatePipelines();
}

auto SplattingRenderer::CreatePipelineInfo() const -> PipelineInfo {
	return {
		.device = createInfo.device,
		.internalTextures = std::make_shared<InternalTextures>(
			createInfo.device,
			createInfo.width,
			createInfo.height,
			createInfo.scale,
			ALBEDO_OUTPUT_SCALE
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
		SimBufferType::VELOCITY0,
		pipelineInfo.internalBuffers->particleVelocities0->GetRawBuffer().Get()
	);

	simData->LinkBuffer(
		SimBufferType::VELOCITY1,
		pipelineInfo.internalBuffers->particleVelocities1->GetRawBuffer().Get()
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

auto SplattingRenderer::RunSurfaceFilteringPipeline(unsigned int iterations)
	-> void {
	if (iterations == 0) {
		const auto context = createInfo.device->GetRawDeviceContext();
		// we'll just want to copy unfilted depth to the filtered depth output
		context->CopyResource(
			pipelineInfo.outputTextures->ellipsoidDepth->GetTexture2D().Get(),
			pipelineInfo.internalTextures->unfilteredEllipsoidDepth
				->GetTexture2D()
				.Get()
		);

		return;
	}

	// we need to only clear the output texture to ensure we don't
	// accidently overwrite the original depth with 1.0
	float depthClearColor[4] = {1.f, 1.f, 1.f, 1.f};

	if (settings.enableSurfaceFiltering) {
		createInfo.device->GetRawDeviceContext()->ClearRenderTargetView(
			pipelineInfo.outputTextures->normals->GetRenderTargetView().Get(),
			depthClearColor
		);
	}

	SetFrameResolution(
		surfaceFilteringA->GetRenderPass()->GetScaledWidth(),
		surfaceFilteringA->GetRenderPass()->GetScaledHeight()
	);

	for (int i = 0; i < iterations; i++) {
		bool oddIteration = i % 2 != 0;
		frameParamCopy.g_SmoothingPassIndex = i;
		pipelineInfo.internalBuffers->fluidRenderCBuffer.UpdateBuffer(
			frameParamCopy
		);

		if (settings.enableSurfaceFiltering) {
			// This helps control the propagation of the normals across the mip
			// chain. If we allow the mip regeneration to happen for every
			// iteration, the depth-based filter quickly becomes overwhelmed by
			// the footprint of smaller mips.
			surfaceFilteringA->GetRenderPass()->SetMipRegenerationEnabled(
				oddIteration
			);
			surfaceFilteringB->GetRenderPass()->SetMipRegenerationEnabled(
				oddIteration
			);

			surfaceFilteringA->Run();
			surfaceFilteringB->Run();
		}
	}
}

auto SplattingRenderer::CreateAbsorptionModifier(
	const std::shared_ptr<Buffer> &absorptionBuffer
) const -> std::shared_ptr<AbsorptionModifier> {
	return std::make_shared<AbsorptionModifier>(
		AbsorptionModifier::AbsorptionModifierCreateInfo{
			.device = createInfo.device, .absorptionBuffer = absorptionBuffer
		}
	);
}

#ifdef GELLY_ENABLE_RENDERDOC_CAPTURES
auto SplattingRenderer::ReloadAllShaders() -> void {
	// gsc hotreloads are global, so we simply just need to remove all the
	// pipelines and remake them
	ReloadAllGSCShaders();
	CreatePipelines();
}

auto SplattingRenderer::InstantiateRenderDoc() -> RENDERDOC_API_1_1_2 * {
	const HMODULE renderDocModule = GetModuleHandle("renderdoc.dll");

	if (renderDocModule == nullptr) {
		return nullptr;
	}

	const auto RENDERDOC_GetAPI = reinterpret_cast<pRENDERDOC_GetAPI>(
		GetProcAddress(renderDocModule, "RENDERDOC_GetAPI")
	);

	if (RENDERDOC_GetAPI == nullptr) {
		return nullptr;
	}

	RENDERDOC_API_1_1_2 *renderDoc = nullptr;
	const auto result = RENDERDOC_GetAPI(
		eRENDERDOC_API_Version_1_1_2, reinterpret_cast<void **>(&renderDoc)
	);

	if (result != 1) {
		return nullptr;
	}

	return renderDoc;
}
#endif

auto SplattingRenderer::CreateQuery() -> ComPtr<ID3D11Query> {
	D3D11_QUERY_DESC queryDesc = {};
	queryDesc.Query = D3D11_QUERY_EVENT;
	queryDesc.MiscFlags = 0;

	ComPtr<ID3D11Query> query;
	createInfo.device->GetRawDevice()->CreateQuery(
		&queryDesc, query.GetAddressOf()
	);

	return query;
}

}  // namespace splatting
}  // namespace renderer
}  // namespace gelly