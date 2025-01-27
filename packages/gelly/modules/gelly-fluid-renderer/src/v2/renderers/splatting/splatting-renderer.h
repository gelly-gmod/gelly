#ifndef SPLATTING_RENDERER_H
#define SPLATTING_RENDERER_H
#include <d3d11_4.h>

#include <memory>

#include "GellyInterfaceRef.h"
#include "device.h"
#include "helpers/rendering/gpu-duration.h"
#include "pipeline/compute-pipeline.h"
#include "pipeline/pipeline.h"
#include "pipelines/pipeline-info.h"
#include "renderdoc_app.h"
#include "resources/buffer-view.h"
#include "v2/solver.h"

namespace gelly {
namespace renderer {
namespace splatting {
using PipelinePtr = std::shared_ptr<Pipeline>;
using ComputePipelinePtr = std::shared_ptr<ComputePipeline>;

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
	static constexpr auto MAX_FRAMES = PipelineInfo::MAX_FRAMES;

	float ALBEDO_OUTPUT_SCALE = 0.5f;

	/**
	 * Timings of all the passes in milliseconds.
	 */
	struct Timings {
		float computeAcceleration = 0.0f;
		float spraySplatting = 0.0f;
		float ellipsoidSplatting = 0.0f;
		float thicknessSplatting = 0.0f;
		float albedoDownsampling = 0.0f;
		float surfaceFiltering = 0.0f;
		float rawNormalEstimation = 0.0f;

		bool isDisjoint = false;
	};

	struct Settings {
		unsigned int filterIterations = 5;
		/**
		 * There is no need to disable this unless you are debugging. Seriously,
		 * if this is disabled then it's purely up to random chance if your
		 * driver will crash or not. But, it is useful for GPU instrumentation
		 * software such as NSight or PIX, as it allows them to properly capture
		 * the frame.
		 */
		bool enableGPUSynchronization = true;
		bool enableSurfaceFiltering = true;
		/**
		 * Should never be true for general use, but this is useful for
		 * debugging on a user's machine. This will enable the GPU timing
		 * queries, which will allow for coarse-grained GPU timing information
		 * to be captured. It is way more in-depth than a simple CPU timing, way
		 * less than a GPU profiler.
		 */
		bool enableGPUTiming = false;
		/**
		 * Enables the various systems that are used to render the whitewater,
		 * which may be disabled for performance reasons.
		 */
		bool enableWhitewater = true;
		float whitewaterStrength = 1.f;
	};

	struct SplattingRendererCreateInfo {
		std::shared_ptr<Device> device;
		simulation::Solver *solver;
		std::array<InputSharedHandles, MAX_FRAMES> inputSharedHandles;

		unsigned int width;
		unsigned int height;
		unsigned int maxParticles;
		float scale = 1.f;
	};

	explicit SplattingRenderer(const SplattingRendererCreateInfo &createInfo);
	~SplattingRenderer() = default;

	static auto Create(const SplattingRendererCreateInfo &&createInfo)
		-> std::shared_ptr<SplattingRenderer>;

	auto Render() -> void;
	auto UpdateFrameParams(cbuffer::FluidRenderCBufferData &data) -> void;
	auto SetFrameResolution(float width, float height) -> void;
	auto GetSettings() const -> Settings;
	auto UpdateSettings(const Settings &settings) -> void;
	auto FetchTimings() -> Timings;
	auto GetOutputD3DBuffers() const -> simulation::OutputD3DBuffers;
	auto GetCurrentFrame() const -> size_t { return currentFrame; }
	auto GetNextFrame() -> size_t { return (currentFrame + 1) % MAX_FRAMES; }

	[[nodiscard]] auto GetAbsorptionModifier() const
		-> std::shared_ptr<AbsorptionModifier>;

	/**
	 * Destroys every single old texture, takes in new shared handles and
	 * resolution + scale.
	 */
	auto UpdateTextureRegistry(
		const std::array<InputSharedHandles, MAX_FRAMES> &inputSharedHandles,
		float width,
		float height,
		float scale
	) -> void;

#ifdef GELLY_ENABLE_RENDERDOC_CAPTURES
	auto ReloadAllShaders() -> void;
#endif
private:
	SplattingRendererCreateInfo createInfo;
	std::shared_ptr<AbsorptionModifier> absorptionModifier;
	Settings settings;
	ComPtr<ID3D11Query> query;

	PipelineInfo pipelineInfo;
	ComputePipelinePtr computeAcceleration;
	std::array<PipelinePtr, MAX_FRAMES> spraySplatting;
	std::array<PipelinePtr, MAX_FRAMES> spraySplattingDepth;
	std::array<PipelinePtr, MAX_FRAMES> ellipsoidSplatting;
	std::array<PipelinePtr, MAX_FRAMES> thicknessSplatting;
	std::array<PipelinePtr, MAX_FRAMES> albedoDownsampling;
	std::array<PipelinePtr, MAX_FRAMES> surfaceFilteringA;
	std::array<PipelinePtr, MAX_FRAMES> surfaceFilteringB;
	std::array<PipelinePtr, MAX_FRAMES> rawNormalEstimation;

	ComPtr<ID3D11Fence> frameFence[MAX_FRAMES] = {};
	ComPtr<ID3D11DeviceContext4> context4;

	size_t fenceValues[MAX_FRAMES] = {};
	size_t currentFrame = 0;

	cbuffer::FluidRenderCBufferData frameParamCopy = {};
#ifdef GELLY_ENABLE_RENDERDOC_CAPTURES
	RENDERDOC_API_1_1_2 *renderDoc = nullptr;
#endif

	struct {
		util::GPUDuration computeAcceleration;
		util::GPUDuration spraySplatting;
		util::GPUDuration sprayDepthSplatting;
		util::GPUDuration ellipsoidSplatting;
		util::GPUDuration thicknessSplatting;
		util::GPUDuration albedoDownsampling;
		util::GPUDuration surfaceFiltering;
		util::GPUDuration rawNormalEstimation;
	} durations;

	Timings latestTimings;

	auto CreatePipelines() -> void;
	auto CreatePipelineInfo() const -> PipelineInfo;

	auto RunSurfaceFilteringPipeline(unsigned int iterations, size_t frameIndex)
		-> void;

#ifdef GELLY_ENABLE_RENDERDOC_CAPTURES
	auto InstantiateRenderDoc() -> RENDERDOC_API_1_1_2 *;
#endif

	auto CreateQuery() -> ComPtr<ID3D11Query>;
	auto CreateAbsorptionModifier(
		const std::shared_ptr<Buffer> &absorptionBuffer
	) const -> std::shared_ptr<AbsorptionModifier>;

	auto RunPipeline(
		PipelinePtr pipeline,
		util::GPUDuration &duration,
		std::optional<int> vertexCount = std::nullopt
	) -> void;
};

}  // namespace splatting
}  // namespace renderer
}  // namespace gelly

#endif	// SPLATTING_RENDERER_H
