#ifndef SPLATTING_RENDERER_H
#define SPLATTING_RENDERER_H
#include <memory>

#include "GellyInterfaceRef.h"
#include "device.h"
#include "fluidsim/ISimData.h"
#include "pipeline/pipeline.h"
#include "pipelines/pipeline-info.h"
#include "renderdoc_app.h"

namespace gelly {
namespace renderer {
namespace splatting {
using PipelinePtr = std::shared_ptr<Pipeline>;

class SplattingRenderer {
public:
	struct Settings {
		unsigned int filterIterations = 2;
	};

	struct SplattingRendererCreateInfo {
		std::shared_ptr<Device> device;
		GellyInterfaceVal<ISimData> simData;
		InputSharedHandles inputSharedHandles;

		unsigned int width;
		unsigned int height;
		unsigned int maxParticles;
	};

	explicit SplattingRenderer(const SplattingRendererCreateInfo &createInfo);
	~SplattingRenderer() = default;

	static auto Create(const SplattingRendererCreateInfo &&createInfo)
		-> std::shared_ptr<SplattingRenderer>;

	auto Render() const -> void;
	auto UpdateFrameParams(cbuffer::FluidRenderCBufferData &data) const -> void;
	auto UpdateSettings(const Settings &settings) -> void;

private:
	SplattingRendererCreateInfo createInfo;
	Settings settings;
	ComPtr<ID3D11Query> query;

	PipelineInfo pipelineInfo;
	PipelinePtr ellipsoidSplatting;
	PipelinePtr depthFilteringA;
	PipelinePtr depthFilteringB;
	PipelinePtr thicknessExtraction;
	PipelinePtr normalEstimation;

#ifdef GELLY_ENABLE_RENDERDOC_CAPTURES
	RENDERDOC_API_1_1_2 *renderDoc = nullptr;
#endif

	auto CreatePipelines() -> void;
	auto CreatePipelineInfo() const -> PipelineInfo;
	auto LinkBuffersToSimData() const -> void;

#ifdef GELLY_ENABLE_RENDERDOC_CAPTURES
	auto InstantiateRenderDoc() -> RENDERDOC_API_1_1_2 *;
#endif

	auto CreateQuery() -> ComPtr<ID3D11Query>;
};

}  // namespace splatting
}  // namespace renderer
}  // namespace gelly

#endif	// SPLATTING_RENDERER_H
