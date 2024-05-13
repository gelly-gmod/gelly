#include "GModCompositor.h"

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

	if (type == PipelineType::STANDARD) {
		pipeline = new StandardPipeline();
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

void GModCompositor::Render() { pipeline->Composite(); }