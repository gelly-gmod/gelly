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
	createInfo(createInfo),
	pipelineInfo(CreatePipelineInfo()),
	query(CreateQuery()) {
	CreatePipelines();
	LinkBuffersToSimData();

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
	depthFilteringA->Run();
	depthFilteringB->Run();
	normalEstimation->Run();
#ifdef GELLY_ENABLE_RENDERDOC_CAPTURES
	if (renderDoc) {
		renderDoc->EndFrameCapture(
			createInfo.device->GetRawDevice().Get(), nullptr
		);
	}
#endif

	createInfo.device->GetRawDeviceContext()->Flush();
	createInfo.device->GetRawDeviceContext()->End(query.Get());

	// busy wait until the query is done
	while (createInfo.device->GetRawDeviceContext()->GetData(
			   query.Get(), nullptr, 0, 0
		   ) == S_FALSE) {
		Sleep(0);
	}
}

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