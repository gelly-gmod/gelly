#include "solver.h"

namespace gelly::simulation {
Solver::Solver(const CreateInfo &createInfo) :
	info(createInfo),
	solver(CreateSolver()),
	params(CreateDefaultParams()),
	scene(CreateScene()),
	buffers(createInfo.library, createInfo.maxParticles),
	outputBuffers() {}

Solver::~Solver() { NvFlexDestroySolver(solver); }

void Solver::Tick(float dt) {
	if (newActiveParticleCount != activeParticleCount) {
		activeParticleCount = newActiveParticleCount;
	}

	dt *= timeStepMultiplier;

	NvFlexCopyDesc copyDesc = {};
	copyDesc.srcOffset = 0;
	copyDesc.dstOffset = 0;
	copyDesc.elementCount = info.maxParticles;

	NvFlexSetParams(solver, &params);
	NvFlexSetActiveCount(solver, activeParticleCount);
	scene.Update();
	NvFlexUpdateSolver(solver, dt, substeps, false);

	NvFlexGetSmoothParticles(
		solver, *outputBuffers.smoothedPositions, &copyDesc
	);

	NvFlexGetAnisotropy(
		solver,
		*outputBuffers.anisotropyQ1,
		*outputBuffers.anisotropyQ2,
		*outputBuffers.anisotropyQ3,
		&copyDesc
	);
}

void Solver::AddParticles(const ParticleBatch &particles) {
	const auto startCount = newActiveParticleCount;
	const auto endCount = startCount + particles.size();

	if (endCount >= info.maxParticles) {
		return;	 // Drop particles
	}

	auto positions = buffers.positions.Map();
	auto velocities = buffers.velocities.Map();
	auto phases = buffers.phases.Map();
	auto actives = buffers.actives.Map();

	for (int index = 0; index < particles.size(); index++) {
		const auto &particle = particles[index];
		positions[index] = {
			particle.position.x, particle.position.y, particle.position.z, 1.0f
		};

		velocities[index] = particle.velocity;

		phases[index] =
			NvFlexMakePhase(0, eNvFlexPhaseSelfCollide | eNvFlexPhaseFluid);

		actives[index] = index;
	}

	newActiveParticleCount = endCount;

	buffers.positions.Unmap();
	buffers.velocities.Unmap();
	buffers.phases.Unmap();
	buffers.actives.Unmap();

	NvFlexCopyDesc copyDesc = {};
	copyDesc.srcOffset = 0;
	copyDesc.dstOffset = 0;
	copyDesc.elementCount = newActiveParticleCount;

	NvFlexSetParticles(solver, *buffers.positions, &copyDesc);
	NvFlexSetVelocities(solver, *buffers.velocities, &copyDesc);
	NvFlexSetPhases(solver, *buffers.phases, &copyDesc);
	NvFlexSetActive(solver, *buffers.actives, &copyDesc);
}

void Solver::Reset() { newActiveParticleCount = 0; }

#define UPDATE_FLEX_PARAM(name, paramName)    \
	if (info.paramName.has_value()) {         \
		params.name = info.paramName.value(); \
	}

void Solver::Update(const UpdateSolverInfo &info) {
	UPDATE_FLEX_PARAM(radius, radius);
	UPDATE_FLEX_PARAM(viscosity, viscosity);
	UPDATE_FLEX_PARAM(cohesion, cohesion);
	if (info.surfaceTension.has_value()) {
		params.surfaceTension = *info.surfaceTension / powf(params.radius, 5.f);
	}
	UPDATE_FLEX_PARAM(vorticityConfinement, vorticityConfinement);
	UPDATE_FLEX_PARAM(adhesion, adhesion);
	UPDATE_FLEX_PARAM(dynamicFriction, dynamicFriction);
	if (info.restDistanceRatio.has_value()) {
		params.fluidRestDistance = *info.restDistanceRatio * params.radius;
	}
	UPDATE_FLEX_PARAM(diffuseBallistic, diffuseBallisticCount);
	UPDATE_FLEX_PARAM(diffuseThreshold, diffuseKineticThreshold);
	UPDATE_FLEX_PARAM(diffuseBuoyancy, diffuseBuoyancy);
	UPDATE_FLEX_PARAM(diffuseDrag, diffuseDrag);
	UPDATE_FLEX_PARAM(diffuseLifetime, diffuseLifetime);
	UPDATE_FLEX_PARAM(numIterations, iterations);
	UPDATE_FLEX_PARAM(relaxationFactor, relaxationFactor);
	UPDATE_FLEX_PARAM(collisionDistance, collisionDistance);
	UPDATE_FLEX_PARAM(gravity[2], gravity);

	if (info.substeps.has_value()) {
		substeps = *info.substeps;
	}
}

int Solver::GetActiveParticleCount() const { return activeParticleCount; }
int Solver::GetCurrentActiveParticleCount() const {
	return newActiveParticleCount;
}

int Solver::GetMaxParticles() const { return info.maxParticles; }
int Solver::GetMaxDiffuseParticles() const { return info.maxDiffuseParticles; }

void Solver::AttachOutputBuffers(const OutputD3DBuffers &buffers) {
	outputBuffers = OutputBuffers(
		{.library = info.library,
		 .maxParticles = info.maxParticles,
		 .maxDiffuseParticles = info.maxDiffuseParticles,
		 .rendererBuffers = buffers}
	);
}

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

Scene Solver::CreateScene() const {
	return Scene(ObjectHandlerContext{.lib = info.library, .solver = solver});
}

}  // namespace gelly::simulation