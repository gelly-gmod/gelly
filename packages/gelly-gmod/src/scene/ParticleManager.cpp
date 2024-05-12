#include "ParticleManager.h"

ParticleListBuilder::ParticleListBuilder()
	: particles(), absorption{0.f, 0.f, 0.f}, absorptionSet(false) {}

ParticleListBuilder ParticleListBuilder::AddParticle(
	const Vector &position, const Vector &velocity
) {
	particles.push_back(::AddParticle{
		position.x, position.y, position.z, velocity.x, velocity.y, velocity.z
	});

	return *this;
}

ParticleListBuilder ParticleListBuilder::SetAbsorption(
	float r, float g, float b
) {
	absorption[0] = r;
	absorption[1] = g;
	absorption[2] = b;
	absorptionSet = true;

	return *this;
}

ParticleManager::ParticleManager(
	const std::shared_ptr<IFluidRenderer> &renderer,
	const std::shared_ptr<IFluidSimulation> &sim
)
	: renderer(renderer), sim(sim) {}

ISimCommandList *ParticleManager::CreateCommandListFromBuilder(
	const ParticleListBuilder &builder
) const {
	auto *cmdList = sim->CreateCommandList();
	for (const auto &particle : builder.particles) {
		cmdList->AddCommand(SimCommand{ADD_PARTICLE, particle});
	}

	return cmdList;
}

void ParticleManager::SubmitPerParticleAbsorption(const ParticleListBuilder &builder
) const {
	int startIndex = sim->GetSimulationData()->GetActiveParticles() + 1;
	int endIndex = startIndex + builder.particles.size();

	for (int i = startIndex; i < endIndex; i++) {
		renderer->SetPerParticleAbsorption(i, builder.absorption);
	}
}

void ParticleManager::PullAbsorptionData(const ParticleListBuilder &builder) const {
	if (builder.absorptionSet &&
		renderer->CheckFeatureSupport(
			GELLY_FEATURE::FLUIDRENDER_PER_PARTICLE_ABSORPTION
		)) {
		renderer->PullPerParticleData();
	}
}

void ParticleManager::PushAbsorptionData(const ParticleListBuilder &builder) const {
	if (builder.absorptionSet &&
		renderer->CheckFeatureSupport(
			GELLY_FEATURE::FLUIDRENDER_PER_PARTICLE_ABSORPTION
		)) {
		renderer->PushPerParticleData();
	}
}

ParticleListBuilder ParticleManager::CreateParticleList() { return {}; }

void ParticleManager::AddParticles(const ParticleListBuilder &builder) const {
	auto *cmdList = CreateCommandListFromBuilder(builder);
	PullAbsorptionData(builder);
	SubmitPerParticleAbsorption(builder);
	PushAbsorptionData(builder);

	sim->ExecuteCommandList(cmdList);
	sim->DestroyCommandList(cmdList);
}

void ParticleManager::ClearParticles() const {
	auto *cmdList = sim->CreateCommandList();
	cmdList->AddCommand(SimCommand{RESET, Reset{}});
	sim->ExecuteCommandList(cmdList);
	sim->DestroyCommandList(cmdList);
}
