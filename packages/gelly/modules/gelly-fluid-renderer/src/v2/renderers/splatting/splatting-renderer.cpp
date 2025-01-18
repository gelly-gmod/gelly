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
	frameQueries({}),
	durations(
		{.computeAcceleration = createInfo.device,
		 .spraySplatting = createInfo.device,
		 .sprayDepthSplatting = createInfo.device,
		 .ellipsoidSplatting = createInfo.device,
		 .thicknessSplatting = createInfo.device,
		 .albedoDownsampling = createInfo.device,
		 .surfaceFiltering = createInfo.device,
		 .rawNormalEstimation = createInfo.device}
	) {
	for (int i = 0; i < MAX_FRAMES; i++) {
		frameQueries[i] = CreateQuery();
	}

	ComPtr<ID3D11Device5> device5;
	const auto castResult = createInfo.device->GetRawDevice().As(&device5);
	if (FAILED(castResult)) {
		throw std::runtime_error("Failed to cast device to ID3D11Device5");
	}

	for (int i = 0; i < MAX_FRAMES; i++) {
		const auto fenceCreationResult = device5->CreateFence(
			0x0, D3D11_FENCE_FLAG_NONE, __uuidof(ID3D11Fence), &frameFence[i]
		);

		if (FAILED(fenceCreationResult)) {
			throw std::runtime_error("Failed to create fence");
		}
	}

	const auto simFenceCreationResult = device5->CreateFence(
		0xFFFFFFFFFFF,
		D3D11_FENCE_FLAG_NONE,
		__uuidof(ID3D11Fence),
		&simulationResourceCopyFence
	);

	if (FAILED(simFenceCreationResult)) {
		throw std::runtime_error(
			"Failed to create simulation resource copy fence"
		);
	}

	const auto dcCastResult =
		createInfo.device->GetRawDeviceContext().As(&context4);

	if (FAILED(dcCastResult)) {
		throw std::runtime_error(
			"Failed to cast device context to ID3D11DeviceContext4"
		);
	}

	CreatePipelines();
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
	if (settings.enableGPUSynchronization) {
		WaitOnSimResources(
		);	// ensures we don't render half-copied simulation data
		ForceWaitForFrameRender(GetCurrentFrame());
	}

#ifdef GELLY_ENABLE_RENDERDOC_CAPTURES
	if (renderDoc) {
		renderDoc->StartFrameCapture(
			createInfo.device->GetRawDevice().Get(), nullptr
		);
	}
#endif
	RunPipeline(
		ellipsoidSplatting[GetCurrentFrame()],
		durations.ellipsoidSplatting,
		createInfo.solver->GetActiveParticleCount()
	);

	if (settings.enableWhitewater) {
		if (settings.enableGPUTiming) {
			durations.computeAcceleration.Start();
		}
		computeAcceleration->Dispatch(
			{static_cast<unsigned int>(
				 createInfo.solver->GetActiveParticleCount()
			 ),
			 1,
			 1}
		);
		if (settings.enableGPUTiming) {
			durations.computeAcceleration.End();
		}

		// populate depth only, the next pass populates density (additive)
		RunPipeline(
			spraySplattingDepth[GetCurrentFrame()],
			durations.sprayDepthSplatting,
			0  // TODO: Reimplement foam
		);

		RunPipeline(
			spraySplatting[GetCurrentFrame()],
			durations.spraySplatting,
			0  // TODO: Reimplement foam
		);
	}

	RunPipeline(
		thicknessSplatting[GetCurrentFrame()],
		durations.thicknessSplatting,
		createInfo.solver->GetActiveParticleCount()
	);

	RunPipeline(
		albedoDownsampling[GetCurrentFrame()], durations.albedoDownsampling
	);
	RunPipeline(
		rawNormalEstimation[GetCurrentFrame()], durations.rawNormalEstimation
	);

	if (settings.enableGPUTiming) {
		durations.surfaceFiltering.Start();
	}

	RunSurfaceFilteringPipeline(settings.filterIterations, GetCurrentFrame());

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
		context4->End(frameQueries[GetCurrentFrame()].Get());
		frameQueriesActive[GetCurrentFrame()] = true;
	}

	if (settings.enableGPUTiming) {
		latestTimings.computeAcceleration =
			durations.computeAcceleration.GetDuration();
		latestTimings.ellipsoidSplatting =
			durations.ellipsoidSplatting.GetDuration();
		latestTimings.spraySplatting =
			durations.spraySplatting.GetDuration() +
			durations.sprayDepthSplatting.GetDuration();
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
								   durations.spraySplatting.IsDisjoint() ||
								   durations.thicknessSplatting.IsDisjoint() ||
								   durations.albedoDownsampling.IsDisjoint() ||
								   durations.surfaceFiltering.IsDisjoint() ||
								   durations.rawNormalEstimation.IsDisjoint();
	}

	currentFrame = GetNextFrame();
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
	pipelineInfo.internalBuffers->fluidRenderCBuffer[GetCurrentFrame()]
		.UpdateBuffer(data);
	std::memcpy(
		&frameParamCopy, &data, sizeof(cbuffer::FluidRenderCBufferData)
	);
}

auto SplattingRenderer::SetFrameResolution(float width, float height) -> void {
	frameParamCopy.g_ViewportWidth = width;
	frameParamCopy.g_ViewportHeight = height;
	frameParamCopy.g_InvViewport.x = 1.f / width;
	frameParamCopy.g_InvViewport.y = 1.f / height;

	pipelineInfo.internalBuffers->fluidRenderCBuffer[GetCurrentFrame()]
		.UpdateBuffer(frameParamCopy);
}

auto SplattingRenderer::CreatePipelines() -> void {
	computeAcceleration = CreateComputeAccelerationPipeline(pipelineInfo);

	for (size_t i = 0; i < MAX_FRAMES; i++) {
		spraySplatting[i] =
			CreateSpraySplattingPipeline(pipelineInfo, i, createInfo.scale);
		spraySplattingDepth[i] = CreateSpraySplattingPipeline(
			pipelineInfo, i, createInfo.scale, true
		);
		ellipsoidSplatting[i] =
			CreateEllipsoidSplattingPipeline(pipelineInfo, i, createInfo.scale);
		thicknessSplatting[i] =
			CreateThicknessSplattingPipeline(pipelineInfo, i, createInfo.scale);
		albedoDownsampling[i] = CreateAlbedoDownsamplingPipeline(
			pipelineInfo, i, ALBEDO_OUTPUT_SCALE
		);

		surfaceFilteringA[i] = CreateSurfaceFilteringPipeline(
			pipelineInfo,
			i,
			pipelineInfo.internalTextures[i]->unfilteredNormals,
			pipelineInfo.outputTextures[i]->normals,
			createInfo.scale
		);

		surfaceFilteringB[i] = CreateSurfaceFilteringPipeline(
			pipelineInfo,
			i,
			pipelineInfo.outputTextures[i]->normals,
			pipelineInfo.internalTextures[i]->unfilteredNormals,
			createInfo.scale
		);

		rawNormalEstimation[i] = CreateNormalEstimationPipeline(
			pipelineInfo,
			i,
			pipelineInfo.outputTextures[i]->ellipsoidDepth,
			pipelineInfo.internalTextures[i]->unfilteredNormals,
			createInfo.scale
		);
	}
}

auto SplattingRenderer::UpdateTextureRegistry(
	const std::array<InputSharedHandles, MAX_FRAMES> &inputSharedHandles,
	float width,
	float height,
	float scale
) -> void {
	createInfo.width = width;
	createInfo.height = height;
	for (size_t i = 0; i < MAX_FRAMES; i++) {
		createInfo.inputSharedHandles[i] = inputSharedHandles[i];
	}
	createInfo.scale = scale;

	pipelineInfo.width = width;
	pipelineInfo.height = height;

	for (size_t i = 0; i < MAX_FRAMES; i++) {
		pipelineInfo.internalTextures[i] = std::make_shared<InternalTextures>(
			createInfo.device,
			createInfo.width,
			createInfo.height,
			createInfo.scale
		);

		pipelineInfo.outputTextures[i] = std::make_shared<OutputTextures>(
			createInfo.device, createInfo.inputSharedHandles[i]
		);
	}

	// Re-initialize all pipelines
	CreatePipelines();
}

auto SplattingRenderer::CreatePipelineInfo() const -> PipelineInfo {
	auto info = PipelineInfo{
		.device = createInfo.device,
		.internalTextures = {},
		.outputTextures = {},
		.internalBuffers = std::make_shared<InternalBuffers>(
			createInfo.device, createInfo.maxParticles
		),
		.width = createInfo.width,
		.height = createInfo.height
	};

	for (size_t i = 0; i < MAX_FRAMES; i++) {
		info.internalTextures[i] = std::make_shared<InternalTextures>(
			createInfo.device, info.width, info.height, createInfo.scale
		);

		info.outputTextures[i] = std::make_shared<OutputTextures>(
			createInfo.device, createInfo.inputSharedHandles[i]
		);
	}

	return info;
}

auto SplattingRenderer::GetOutputD3DBuffers() const
	-> simulation::OutputD3DBuffers {
	return {
		.smoothedPositions =
			pipelineInfo.internalBuffers->particlePositions->GetRawBuffer().Get(
			),
		.velocitiesPrevFrame =
			pipelineInfo.internalBuffers->particleVelocities0->GetRawBuffer()
				.Get(),
		.velocities =
			pipelineInfo.internalBuffers->particleVelocities1->GetRawBuffer()
				.Get(),
		.anisotropyQ1 =
			pipelineInfo.internalBuffers->anisotropyQ1->GetRawBuffer().Get(),
		.anisotropyQ2 =
			pipelineInfo.internalBuffers->anisotropyQ2->GetRawBuffer().Get(),
		.anisotropyQ3 =
			pipelineInfo.internalBuffers->anisotropyQ3->GetRawBuffer().Get(),
		.foamPositions =
			pipelineInfo.internalBuffers->foamPositions->GetRawBuffer().Get(),
		.foamVelocities =
			pipelineInfo.internalBuffers->foamVelocities->GetRawBuffer().Get(),
	};
}

auto SplattingRenderer::RunSurfaceFilteringPipeline(
	unsigned int iterations, size_t frameIndex
) -> void {
	if (iterations == 0) {
		const auto context = createInfo.device->GetRawDeviceContext();
		// we'll just want to copy unfilted depth to the filtered depth output
		context->CopyResource(
			pipelineInfo.outputTextures[frameIndex]
				->ellipsoidDepth->GetTexture2D()
				.Get(),
			pipelineInfo.internalTextures[frameIndex]
				->unfilteredEllipsoidDepth->GetTexture2D()
				.Get()
		);

		return;
	}

	// we need to only clear the output texture to ensure we don't
	// accidently overwrite the original depth with 1.0
	float depthClearColor[4] = {1.f, 1.f, 1.f, 1.f};

	if (settings.enableSurfaceFiltering) {
		createInfo.device->GetRawDeviceContext()->ClearRenderTargetView(
			pipelineInfo.outputTextures[frameIndex]
				->normals->GetRenderTargetView()
				.Get(),
			depthClearColor
		);
	}

	SetFrameResolution(
		surfaceFilteringA[frameIndex]->GetRenderPass()->GetScaledWidth(),
		surfaceFilteringA[frameIndex]->GetRenderPass()->GetScaledHeight()
	);

	for (int i = 0; i < iterations; i++) {
		bool oddIteration = i % 2 != 0;
		frameParamCopy.g_SmoothingPassIndex = i;
		pipelineInfo.internalBuffers->fluidRenderCBuffer[GetCurrentFrame()]
			.UpdateBuffer(frameParamCopy);

		if (settings.enableSurfaceFiltering) {
			// This helps control the propagation of the normals across the mip
			// chain. If we allow the mip regeneration to happen for every
			// iteration, the depth-based filter quickly becomes overwhelmed by
			// the footprint of smaller mips.
			surfaceFilteringA[frameIndex]
				->GetRenderPass()
				->SetMipRegenerationEnabled(oddIteration);
			surfaceFilteringB[frameIndex]
				->GetRenderPass()
				->SetMipRegenerationEnabled(oddIteration);

			surfaceFilteringA[frameIndex]->Run();
			surfaceFilteringB[frameIndex]->Run();
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

auto SplattingRenderer::RunPipeline(
	PipelinePtr pipeline,
	util::GPUDuration &duration,
	const std::optional<int> vertexCount
) -> void {
	if (settings.enableGPUTiming) {
		duration.Start();
	}

	SetFrameResolution(
		pipeline->GetRenderPass()->GetScaledWidth(),
		pipeline->GetRenderPass()->GetScaledHeight()
	);

	pipeline->Run(vertexCount);

	if (settings.enableGPUTiming) {
		duration.End();
	}
}

}  // namespace splatting
}  // namespace renderer
}  // namespace gelly