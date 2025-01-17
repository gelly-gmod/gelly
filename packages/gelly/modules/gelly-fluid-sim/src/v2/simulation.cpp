#include "simulation.h"

namespace gelly::simulation {
Simulation::Simulation(const CreateInfo &createInfo) :
	info(createInfo),
	library(CreateLibrary()),
	solver(
		{.library = library,
		 .maxParticles = createInfo.maxParticles,
		 .maxDiffuseParticles = createInfo.maxDiffuseParticles,
		 .rendererBuffers = createInfo.rendererBuffers}
	) {}

Simulation::~Simulation() {
	if (library) {
		NvFlexShutdown(library);
	}
}

NvFlexLibrary *Simulation::CreateLibrary() const {
	NvFlexInitDesc desc = {};
	desc.renderDevice = info.device;
	desc.renderContext = info.context;
	desc.enableExtensions = false;
	desc.runOnRenderContext = true;

	return NvFlexInit(NV_FLEX_VERSION, ErrorHandler, &desc);
}
}  // namespace gelly::simulation