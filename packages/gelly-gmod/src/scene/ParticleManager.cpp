#include "ParticleManager.h"

ParticleListBuilder::ParticleListBuilder() :
	particles(), absorption{0.f, 0.f, 0.f}, absorptionSet(false) {}

ParticleListBuilder ParticleListBuilder::AddParticle(
	const Vector &position, const Vector &velocity
) {
	particles.push_back(
		{.position = XMFLOAT3(position.x, position.y, position.z),
		 .velocity = XMFLOAT3(velocity.x, velocity.y, velocity.z)}
	);

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
	const std::shared_ptr<gelly::simulation::Simulation> &sim
) :
	sim(sim) {}

ParticleListBuilder ParticleManager::CreateParticleList() { return {}; }

void ParticleManager::AddParticles(
	const ParticleListBuilder &builder,
	const std::shared_ptr<gelly::renderer::splatting::AbsorptionModifier>
		&absorptionModifier
) const {
	absorptionModifier->StartModifying();
	for (int i = 0; i < builder.particles.size(); ++i) {
		if (sim->GetSolver().GetCurrentActiveParticleCount() + i >=
			sim->GetSolver().GetMaxParticles()) {
			break;
		}

		absorptionModifier->ModifyAbsorption(
			sim->GetSolver().GetCurrentActiveParticleCount() + i,
			reinterpret_cast<const gelly::renderer::splatting::float3 &>(
				builder.absorption
			)
		);
	}

	absorptionModifier->EndModifying();
	sim->GetSolver().AddParticles(builder.particles);
}

void ParticleManager::ClearParticles() const { sim->GetSolver().Reset(); }
