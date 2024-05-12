#include "GModCompositor.h"

#include "Pipeline.h"
#include "standard/StandardPipeline.h"

GModCompositor::GModCompositor(
	PipelineType type,
	GellyInterfaceVal<IFluidRenderer> renderer,
	GellyInterfaceVal<IRenderContext> context
)
	: pipeline(nullptr), gellyResources(), resources() {
	gellyResources.renderer = std::shared_ptr<IFluidRenderer>(renderer);
	gellyResources.context = std::shared_ptr<IRenderContext>(context);
	gellyResources.textures = renderer->GetFluidTextures();

	if (type == PipelineType::STANDARD) {
		pipeline = new StandardPipeline();
		pipeline->CreatePipelineLocalResources(
			gellyResources, resources.FindGModResources()
		);
	}
}

void GModCompositor::SetConfig(PipelineConfig config) {
	pipeline->SetConfig(config);
}

void GModCompositor::Render() { pipeline->Composite(); }