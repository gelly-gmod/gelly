#include "solver.h"

namespace gelly::simulation {
Solver::Solver(const CreateInfo &createInfo) :
	info(createInfo),
	solver(CreateSolver()),
	params({}),
	scene(CreateScene()),
	buffers(createInfo.library, createInfo.maxParticles),
	outputBuffers() {
	SetupDefaultParams();
}

Solver::~Solver() {
	if (solver) {
	}
}

void Solver::BeginTick(float dt) {
	particleCountAtBeginTick = newActiveParticleCount;
	dt *= timeStepMultiplier;
	lastDeltaTime = dt;

	NvFlexSetParams(solver, &params);
	NvFlexSetActiveCount(solver, newActiveParticleCount);
	scene.Update();
	NvFlexUpdateSolver(solver, dt, substeps, false);
}

void Solver::EndTick() {
	NvFlexCopyDesc copyDesc = {};
	copyDesc.srcOffset = 0;
	copyDesc.dstOffset = 0;
	copyDesc.elementCount = newActiveParticleCount;

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

	if (IsWhitewaterEnabled()) {
		NvFlexGetDiffuseParticles(
			solver,
			*outputBuffers.foamPositions,
			*outputBuffers.foamVelocities,
			*buffers.diffuseParticleCount
		);

		NvFlexGetVelocities(
			solver, *outputBuffers.velocities[velocityFrameIndex], &copyDesc
		);

		velocityFrameIndex = (velocityFrameIndex + 1) % VELOCITY_FRAMES;

		const auto *count = buffers.diffuseParticleCount.Map();
		activeDiffuseParticleCount = *count;
		buffers.diffuseParticleCount.Unmap();
	}

	activeParticleCount = particleCountAtBeginTick;
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
		positions[index + startCount] = {
			particle.position.x,
			particle.position.y,
			particle.position.z,
			particle.invMass
		};

		velocities[index + startCount] = particle.velocity;

		phases[index + startCount] =
			NvFlexMakePhase(0, eNvFlexPhaseSelfCollide | eNvFlexPhaseFluid);

		actives[index + startCount] = index + startCount;
	}

	newActiveParticleCount = endCount;

	buffers.positions.Unmap();
	buffers.velocities.Unmap();
	buffers.phases.Unmap();
	buffers.actives.Unmap();

	NvFlexCopyDesc copyDesc = {};
	copyDesc.srcOffset = startCount;
	copyDesc.dstOffset = startCount;
	copyDesc.elementCount = particles.size();

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
	if (info.diffuseLifetime.has_value()) {
		diffuseLifetime = *info.diffuseLifetime;
		params.diffuseLifetime = diffuseLifetime * timeStepMultiplier;
	}

	UPDATE_FLEX_PARAM(numIterations, iterations);
	UPDATE_FLEX_PARAM(relaxationFactor, relaxationFactor);
	UPDATE_FLEX_PARAM(collisionDistance, collisionDistance);
	UPDATE_FLEX_PARAM(gravity[2], gravity);

	if (info.substeps.has_value()) {
		substeps = *info.substeps;
	}

	UPDATE_FLEX_PARAM(anisotropyMin, anisotropyMin);
	UPDATE_FLEX_PARAM(anisotropyMax, anisotropyMax);
}

int Solver::GetActiveParticleCount() const { return activeParticleCount; }
int Solver::GetActiveDiffuseParticleCount() const {
	return activeDiffuseParticleCount;
}
int Solver::GetCurrentActiveParticleCount() const {
	return newActiveParticleCount;
}

int Solver::GetMaxParticles() const { return info.maxParticles; }
int Solver::GetMaxDiffuseParticles() const { return info.maxDiffuseParticles; }

void Solver::AttachOutputBuffers(const OutputD3DBuffers &buffers) {
	outputBuffers = OutputBuffers({
		.rendererBuffers = buffers,
		.library = info.library,
		.maxParticles = info.maxParticles,
		.maxDiffuseParticles = info.maxDiffuseParticles,
	});
}

NvFlexSolver *Solver::CreateSolver() const {
	NvFlexSolverDesc desc = {};
	NvFlexSetSolverDescDefaults(&desc);

	desc.maxParticles = info.maxParticles;
	desc.maxDiffuseParticles = info.maxDiffuseParticles;

	return NvFlexCreateSolver(info.library, &desc);
}

void Solver::SetupDefaultParams() {
	params.gravity[0] = 0.f;
	params.gravity[1] = 0.f;
	// Z component is configured by the user

	params.viscosity = 0.0f;
	params.dynamicFriction = 0.1f;
	params.staticFriction = 1.f;
	params.particleFriction = 0.1f;
	params.freeSurfaceDrag = 0.0f;
	params.drag = 0.0f;
	params.lift = 0.0f;
	params.numIterations = 3;

	params.fluidRestDistance = params.radius * 0.73f;
	params.solidRestDistance = params.radius * 2.13f;

	if (params.surfaceTension > 0.f) {
		params.surfaceTension =
			params.surfaceTension / powf(params.radius, 5.f);
	}

	params.anisotropyScale = 1.0f;
	params.anisotropyMin = 0.1f;
	params.anisotropyMax = 2.0f;
	params.smoothing = 1.f;

	params.dissipation = 0.0f;
	params.damping = 0.0f;
	params.particleCollisionMargin = 0.f;
	params.shapeCollisionMargin = 0.4f;
	params.sleepThreshold = 0.0f;
	params.shockPropagation = 0.0f;
	params.restitution = 1.0f;

	params.maxSpeed = FLT_MAX;
	params.maxAcceleration = 100.0f;  // approximately 10x gravity

	params.relaxationMode = eNvFlexRelaxationLocal;
	params.solidPressure = 0.5f;
	params.adhesion = 0.0f;
	params.cohesion = 0.02f;
	params.surfaceTension = 1.0f;
	params.vorticityConfinement = 1.0f;
	params.buoyancy = 1.0f;

	params.diffuseLifetime = diffuseLifetime * timeStepMultiplier;
}

Scene Solver::CreateScene() const {
	return Scene(ObjectHandlerContext{.lib = info.library, .solver = solver});
}

}  // namespace gelly::simulation