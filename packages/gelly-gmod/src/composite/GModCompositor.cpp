#include "GModCompositor.h"

#include "../logging/global-macros.h"
#include "Pipeline.h"
#include "standard/StandardPipeline.h"

GModCompositor::GModCompositor(
	PipelineType type, const std::shared_ptr<ISimData> &simData,
) :
	pipeline(nullptr), gellyResources() {
	using namespace gelly::renderer::splatting;

	gellyResources.device = std::make_shared<gelly::renderer::Device>();

	if (type == PipelineType::STANDARD) {
		pipeline = std::make_unique<StandardPipeline>();
		auto sharedHandles = pipeline->CreatePipelineLocalResources(
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