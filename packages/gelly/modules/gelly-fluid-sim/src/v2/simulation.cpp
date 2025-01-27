#include "simulation.h"

namespace gelly::simulation {
Simulation::Simulation(const CreateInfo &createInfo) :
	info(createInfo),
	library(CreateLibrary()),
	solver({
		.library = library,
		.maxParticles = createInfo.maxParticles,
		.maxDiffuseParticles = createInfo.maxDiffuseParticles,
	}) {}

Simulation::~Simulation() {
	if (library) {
	}
}

void Simulation::AttachOutputBuffers(
	const std::array<OutputD3DBuffers, renderer::splatting::MAX_FRAMES> &buffers
) {
	solver.AttachOutputBuffers(buffers);
}

NvFlexLibrary *Simulation::CreateLibrary() const {
	NvFlexInitDesc desc = {};
	desc.renderDevice = info.device;
	desc.enableExtensions = true;
	desc.runOnRenderContext = true;
	desc.computeType = eNvFlexD3D11;

	return NvFlexInit(NV_FLEX_VERSION, ErrorHandler, &desc);
}

void Simulation::ErrorHandler(
	NvFlexErrorSeverity severity, const char *msg, const char *file, int line
) {
	switch (severity) {
		case eNvFlexLogError:
			printf("Simulation::ErrorHandler: %s (%s:%d)\n", msg, file, line);
			exit(1);
			break;
		default:
			printf(
				"Simulation::ErrorHandler (non-fatal): %s (%s:%d)\n",
				msg,
				file,
				line
			);
	}
}

}  // namespace gelly::simulation