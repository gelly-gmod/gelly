#ifndef SPLATTING_RENDERER_H
#define SPLATTING_RENDERER_H
#include <memory>

#include "GellyInterfaceRef.h"
#include "device.h"
#include "fluidsim/ISimData.h"
#include "pipeline/pipeline.h"
#include "pipelines/pipeline-info.h"
#include "renderdoc_app.h"
#include "resources/buffer-view.h"

namespace gelly {
namespace renderer {
namespace splatting {
using PipelinePtr = std::shared_ptr<Pipeline>;

class AbsorptionModifier {
public:
	struct AbsorptionModifierCreateInfo {
		std::shared_ptr<Device> device;
		std::shared_ptr<Buffer> absorptionBuffer;
	};

	explicit AbsorptionModifier(const AbsorptionModifierCreateInfo &createInfo);
	~AbsorptionModifier() = default;

	auto StartModifying() -> void;
	auto ModifyAbsorption(int particleIndex, float3 absorption) -> void;
	auto EndModifying() -> void;

private:
	AbsorptionModifierCreateInfo createInfo;
	std::shared_ptr<BufferView> bufferView;
};

class SplattingRenderer {
public:
	struct Settings {
		unsigned int filterIterations = 1;
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
	[[nodiscard]] auto GetAbsorptionModifier() const
		-> std::shared_ptr<AbsorptionModifier>;

private:
	SplattingRendererCreateInfo createInfo;
	std::shared_ptr<AbsorptionModifier> absorptionModifier;
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

	auto RunDepthSmoothingFilter(int iterations) const -> void;

#ifdef GELLY_ENABLE_RENDERDOC_CAPTURES
	auto InstantiateRenderDoc() -> RENDERDOC_API_1_1_2 *;
#endif

	auto CreateQuery() -> ComPtr<ID3D11Query>;
	auto CreateAbsorptionModifier(
		const std::shared_ptr<Buffer> &absorptionBuffer
	) const -> std::shared_ptr<AbsorptionModifier>;
};

}  // namespace splatting
}  // namespace renderer
}  // namespace gelly

#endif	// SPLATTING_RENDERER_H
