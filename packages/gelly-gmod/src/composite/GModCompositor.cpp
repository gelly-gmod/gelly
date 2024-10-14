#include "GModCompositor.h"

#include "../logging/global-macros.h"
#include "Pipeline.h"
#ifdef GELLY_ENABLE_RENDERDOC_CAPTURES
#include "reload-shaders.h"
#endif
#include "standard/StandardPipeline.h"

GModCompositor::GModCompositor(
	PipelineType type,
	GellyInterfaceVal<ISimData> simData,
	const std::shared_ptr<gelly::renderer::Device> &device,
	unsigned int width,
	unsigned int height,
	unsigned int maxParticles,
	float scale
) :
	pipeline(nullptr), gellyResources(), width(width), height(height) {
	using namespace gelly::renderer::splatting;

	gellyResources.device = device;

	if (type == PipelineType::STANDARD) {
		pipeline = std::make_unique<StandardPipeline>(width, height);
		const auto sharedHandles = pipeline->CreatePipelineLocalResources(
			gellyResources, Resources::FindGModResources(), width, height, scale
		);

		gellyResources.splattingRenderer = SplattingRenderer::Create(
			{.device = gellyResources.device,
			 .simData = simData,
			 .inputSharedHandles = sharedHandles,
			 .width = width,
			 .height = height,
			 .maxParticles = maxParticles,
			 .scale = scale}
		);

		pipeline->UpdateGellyResources(gellyResources);
	}
}

void GModCompositor::SetConfig(PipelineConfig config) {
	pipeline->SetConfig(config);
}

[[nodiscard]] PipelineConfig GModCompositor::GetConfig() const {
	return pipeline->GetConfig();
}

void GModCompositor::SetFluidMaterial(const PipelineFluidMaterial &material) {
	pipeline->SetFluidMaterial(material);
}

void GModCompositor::Composite() { pipeline->Composite(); }

void GModCompositor::Render() { pipeline->Render(); }

#ifdef GELLY_ENABLE_RENDERDOC_CAPTURES
void GModCompositor::ReloadAllShaders() {
	gellyResources.splattingRenderer->ReloadAllShaders();
	gelly::gmod::renderer::ReloadAllGSCShaders();
	pipeline->ReloadAllShaders();
}
#endif