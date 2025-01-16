#include "solver.h"

namespace gelly::simulation {
Solver::Solver(const CreateInfo &createInfo) :
	info(createInfo),
	solver(CreateSolver()),
	params(CreateDefaultParams()),
	buffers(createInfo.library, createInfo.maxParticles),
	outputBuffers(
		{.library = createInfo.library,
		 .maxParticles = createInfo.maxParticles,
		 .maxDiffuseParticles = createInfo.maxDiffuseParticles,
		 .rendererBuffers = createInfo.rendererBuffers}
	) {}

Solver::~Solver() { NvFlexDestroySolver(solver); }

NvFlexSolver *Solver::CreateSolver() const {
	NvFlexSolverDesc desc = {};
	NvFlexSetSolverDescDefaults(&desc);

	desc.maxParticles = info.maxParticles;
	desc.maxDiffuseParticles = info.maxDiffuseParticles;

	return NvFlexCreateSolver(info.library, &desc);
}

NvFlexParams Solver::CreateDefaultParams() {
	NvFlexParams params = {};
	// TODO: Add default params

	return params;
}

}  // namespace gelly::simulation