#include "GModCompositor.h"

#include "../logging/global-macros.h"
#include "Pipeline.h"
#include "standard/StandardPipeline.h"

GModCompositor::GModCompositor(
	PipelineType type,
	std::shared_ptr<IFluidRenderer> renderer,
	std::shared_ptr<IRenderContext> context
)
	: pipeline(nullptr), gellyResources() {
	gellyResources.renderer = renderer;
	gellyResources.context = context;
	gellyResources.textures = renderer->GetFluidTextures();

#ifdef GELLY_ENABLE_RENDERDOC_CAPTURES
	LOG_INFO("GPU debugging detected, enabling RenderDoc integration...");
	if (const auto success = renderer->EnableRenderDocCaptures(); !success) {
		LOG_WARNING(
			"Failed to enable captures, maybe RenderDoc is not running "
			"or "
			"the API has changed?"
		);
	} else {
		LOG_INFO("RenderDoc captures enabled");
	}
#endif

	if (type == PipelineType::STANDARD) {
		pipeline = std::make_unique<StandardPipeline>();
		pipeline->CreatePipelineLocalResources(
			gellyResources, Resources::FindGModResources()
		);
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