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

public:
	GModCompositor(
		PipelineType type,
		GellyInterfaceVal<ISimData> simData,
		const std::shared_ptr<gelly::renderer::Device> &device,
		unsigned int width,
		unsigned int height,
		unsigned int maxParticles
	);

	GModCompositor(const GModCompositor &) = delete;
	GModCompositor &operator=(const GModCompositor &) = delete;
	GModCompositor(GModCompositor &&) = delete;
	GModCompositor &operator=(GModCompositor &&) = delete;

	~GModCompositor() {
		LOG_INFO("GModCompositor destructor called");
		// we dont need to delete pipeline because it is a unique_ptr
	}

	void SetConfig(PipelineConfig config);
	[[nodiscard]] PipelineConfig GetConfig() const;
	void SetFluidMaterial(const PipelineFluidMaterial &material);

	void Composite();
	void Render();

	[[nodiscard]] unsigned int GetWidth() const { return width; }

	[[nodiscard]] unsigned int GetHeight() const { return height; }

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

	void UpdateGellySettings(const SplattingRenderer::Settings &settings
	) const {
		gellyResources.splattingRenderer->UpdateSettings(settings);
	}
};

#endif	// COMPOSITE_H
