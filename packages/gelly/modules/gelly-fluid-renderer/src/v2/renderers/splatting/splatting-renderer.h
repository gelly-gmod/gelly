#ifndef SPLATTING_RENDERER_H
#define SPLATTING_RENDERER_H
#include <memory>

#include "GellyInterfaceRef.h"
#include "device.h"
#include "fluidsim/ISimData.h"
#include "pipeline/pipeline.h"
#include "pipelines/pipeline-info.h"

namespace gelly {
namespace renderer {
namespace splatting {

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

	auto Render() const -> void;
	auto UpdateFrameParams(const cbuffer::FluidRenderCBufferData &data) const -> void;
	auto UpdateSettings(const Settings &settings) -> void;

private:
	using PipelinePtr = std::shared_ptr<Pipeline>;

	SplattingRendererCreateInfo createInfo;
	Settings settings;

	PipelineInfo pipelineInfo;
	PipelinePtr ellipsoidSplatting;
	PipelinePtr depthFilteringA;
	PipelinePtr depthFilteringB;
	PipelinePtr normalEstimation;

	auto CreatePipelines() -> void;
	auto CreatePipelineInfo() const -> PipelineInfo;
	auto LinkBuffersToSimData() const -> void;
};

}  // namespace splatting
}  // namespace renderer
}  // namespace gelly

#endif	// SPLATTING_RENDERER_H
