#include "splatting-renderer.h"

#include "pipelines/depth-filtering.h"
#include "pipelines/ellipsoid-splatting.h"
#include "pipelines/normal-estimation.h"
#include "pipelines/thickness-extraction.h"

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
	query(CreateQuery()) {
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

auto SplattingRenderer::Render() const -> void {
#ifdef GELLY_ENABLE_RENDERDOC_CAPTURES
	if (renderDoc) {
		renderDoc->StartFrameCapture(
			createInfo.device->GetRawDevice().Get(), nullptr
		);
	}
#endif
	ellipsoidSplatting->Run(createInfo.simData->GetActiveParticles());
	thicknessExtraction->Run();
	RunDepthSmoothingFilter(settings.filterIterations);
	frontNormalEstimation->Run();
#ifdef GELLY_ENABLE_RENDERDOC_CAPTURES
	if (renderDoc) {
		renderDoc->EndFrameCapture(
			createInfo.device->GetRawDevice().Get(), nullptr
		);
	}
#endif

	if (settings.enableGPUSynchronization) {
		createInfo.device->GetRawDeviceContext()->Flush();
		createInfo.device->GetRawDeviceContext()->End(query.Get());

		// busy wait until the query is done
		while (createInfo.device->GetRawDeviceContext()->GetData(
				   query.Get(), nullptr, 0, 0
			   ) == S_FALSE) {
			Sleep(0);
		}
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

auto SplattingRenderer::UpdateFrameParams(cbuffer::FluidRenderCBufferData &data
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
	backDepthFilteringA = CreateDepthFilteringPipeline(
		pipelineInfo,
		pipelineInfo.internalTextures->unfilteredBackEllipsoidDepth,
		pipelineInfo.internalTextures->filteredBackEllipsoidDepth
	);
	backDepthFilteringB = CreateDepthFilteringPipeline(
		pipelineInfo,
		pipelineInfo.internalTextures->filteredBackEllipsoidDepth,
		pipelineInfo.internalTextures->unfilteredBackEllipsoidDepth
	);

	frontNormalEstimation = CreateNormalEstimationPipeline(
		pipelineInfo,
		pipelineInfo.internalTextures->unfilteredEllipsoidDepth,
		pipelineInfo.outputTextures->normals,
		true
	);

	thicknessExtraction = CreateThicknessExtractionPipeline(pipelineInfo);
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

auto SplattingRenderer::RunDepthSmoothingFilter(unsigned int iterations) const
	-> void {
	// we need to only clear the output texture to ensure we don't
	// accidently overwrite the original depth with 1.0
	float depthClearColor[4] = {1.f, 1.f, 1.f, 1.f};

	if (settings.enableFrontDepthFiltering) {
		createInfo.device->GetRawDeviceContext()->ClearRenderTargetView(
			pipelineInfo.outputTextures->ellipsoidDepth->GetRenderTargetView()
				.Get(),
			depthClearColor
		);
	}

	if (settings.enableBackDepthFiltering) {
		createInfo.device->GetRawDeviceContext()->ClearRenderTargetView(
			pipelineInfo.internalTextures->filteredBackEllipsoidDepth
				->GetRenderTargetView()
				.Get(),
			depthClearColor
		);
	}

	for (int i = 0; i < iterations; i++) {
		if (settings.enableFrontDepthFiltering) {
			depthFilteringA->Run();
			depthFilteringB->Run();
		}

		if (settings.enableBackDepthFiltering) {
			backDepthFilteringA->Run();
			backDepthFilteringB->Run();
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