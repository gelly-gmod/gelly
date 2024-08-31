#include "ParticleManager.h"

ParticleListBuilder::ParticleListBuilder() :
	particles(), absorption{0.f, 0.f, 0.f}, absorptionSet(false) {}

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

ParticleManager::ParticleManager(const std::shared_ptr<IFluidSimulation> &sim) :
	sim(sim) {}

ISimCommandList *ParticleManager::CreateCommandListFromBuilder(
	const ParticleListBuilder &builder
) const {
	auto *cmdList = sim->CreateCommandList();
	for (const auto &particle : builder.particles) {
		cmdList->AddCommand(SimCommand{ADD_PARTICLE, particle});
	}

	return cmdList;
}

ParticleListBuilder ParticleManager::CreateParticleList() { return {}; }

void ParticleManager::AddParticles(
	const ParticleListBuilder &builder,
	const std::shared_ptr<gelly::renderer::splatting::AbsorptionModifier>
		&absorptionModifier
) const {
	auto *cmdList = CreateCommandListFromBuilder(builder);

	absorptionModifier->StartModifying();
	for (int i = 0; i < builder.particles.size(); ++i) {
		if (sim->GetRealActiveParticleCount() + i >=
			sim->GetSimulationData()->GetMaxParticles()) {
			break;
		}

		absorptionModifier->ModifyAbsorption(
			sim->GetRealActiveParticleCount() + i,
			reinterpret_cast<const gelly::renderer::splatting::float3 &>(
				builder.absorption
			)
		);
	}
	absorptionModifier->EndModifying();

	sim->ExecuteCommandList(cmdList);
	sim->DestroyCommandList(cmdList);
}

void ParticleManager::ClearParticles() const {
	auto *cmdList = sim->CreateCommandList();
	cmdList->AddCommand(SimCommand{RESET, Reset{}});
	sim->ExecuteCommandList(cmdList);
	sim->DestroyCommandList(cmdList);
}
