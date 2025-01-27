#ifndef COMPOSITE_H
#define COMPOSITE_H
#include "Pipeline.h"
#include "PipelineType.h"
#include "Resources.h"
#include "logging/global-macros.h"

class GModCompositor {
private:
	std::unique_ptr<Pipeline> pipeline;
	GellyResources gellyResources;

	unsigned int width;
	unsigned int height;
	float scale;

public:
	GModCompositor(
		PipelineType type,
		gelly::simulation::Solver *solver,
		const std::shared_ptr<gelly::renderer::Device> &device,
		unsigned int width,
		unsigned int height,
		unsigned int maxParticles,
		float scale
	);

	GModCompositor(const GModCompositor &) = delete;
	GModCompositor &operator=(const GModCompositor &) = delete;
	GModCompositor(GModCompositor &&) = delete;
	GModCompositor &operator=(GModCompositor &&) = delete;

	~GModCompositor() = default;

	void SetConfig(PipelineConfig config);
	[[nodiscard]] PipelineConfig GetConfig() const;
	void SetFluidMaterial(const PipelineFluidMaterial &material);

	void Composite();
	void Render();

	void ChangeResolution(
		unsigned int newWidth, unsigned int newHeight, float scale
	) {
		auto newHandles = pipeline->CreatePipelineLocalResources(
			gellyResources,
			Resources::FindGModResources(),
			newWidth,
			newHeight,
			scale
		);

		gellyResources.splattingRenderer->UpdateTextureRegistry(
			newHandles, newWidth, newHeight, scale
		);

		width = newWidth;
		height = newHeight;
		this->scale = scale;
	}
#ifdef GELLY_ENABLE_RENDERDOC_CAPTURES
	void ReloadAllShaders();
#endif

	[[nodiscard]] unsigned int GetWidth() const { return width; }

	[[nodiscard]] unsigned int GetHeight() const { return height; }

	[[nodiscard]] float GetScale() const { return scale; }

	// any non-const access to the GellyResources should be avoided
	[[nodiscard]] const GellyResources &GetGellyResources() const {
		return gellyResources;
	}

	[[nodiscard]] std::shared_ptr<
		gelly::renderer::splatting::AbsorptionModifier>
	GetAbsorptionModifier() const {
		return gellyResources.splattingRenderer->GetAbsorptionModifier();
	}

	[[nodiscard]] SplattingRenderer::Settings GetGellySettings() const {
		return gellyResources.splattingRenderer->GetSettings();
	}

	void UpdateGellySettings(
		const SplattingRenderer::Settings &settings
	) const {
		gellyResources.splattingRenderer->UpdateSettings(settings);
	}

	[[nodiscard]] SplattingRenderer::Timings FetchGellyTimings() const {
		return gellyResources.splattingRenderer->FetchTimings();
	}

	[[nodiscard]] std::array<
		gelly::simulation::OutputD3DBuffers,
		SplattingRenderer::MAX_FRAMES>
	GetOutputD3DBuffers() const {
		return gellyResources.splattingRenderer->GetOutputD3DBuffers();
	}
};

#endif	// COMPOSITE_H
