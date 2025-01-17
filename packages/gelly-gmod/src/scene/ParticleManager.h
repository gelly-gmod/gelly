#ifndef PARTICLES_H
#define PARTICLES_H
#include <memory>
#include <vector>

#include "GarrysMod/Lua/SourceCompat.h"
#include "fluidsim/IFluidSimulation.h"
#include "fluidsim/ISimCommandList.h"
#include "renderers/splatting/splatting-renderer.h"
#include "v2/simulation.h"

class ParticleListBuilder {
	friend class ParticleManager;

protected:
	gelly::simulation::ParticleBatch particles;
	float absorption[3];
	bool absorptionSet = false;

public:
	ParticleListBuilder();
	~ParticleListBuilder() = default;

	ParticleListBuilder AddParticle(
		const Vector &position, const Vector &velocity
	);

	ParticleListBuilder SetAbsorption(float r, float g, float b);
};

class ParticleManager {
private:
	std::shared_ptr<gelly::simulation::Simulation> sim;

public:
	explicit ParticleManager(
		const std::shared_ptr<gelly::simulation::Simulation> &sim
	);
	~ParticleManager() = default;

	static ParticleListBuilder CreateParticleList();
	void AddParticles(
		const ParticleListBuilder &builder,
		const std::shared_ptr<gelly::renderer::splatting::AbsorptionModifier>
			&absorptionModifier
	) const;
	void ClearParticles() const;
};

#endif	// PARTICLES_H
