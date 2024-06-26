#ifndef PARTICLES_H
#define PARTICLES_H
#include <memory>
#include <vector>

#include "GarrysMod/Lua/SourceCompat.h"
#include "fluidrender/IFluidRenderer.h"
#include "fluidsim/IFluidSimulation.h"
#include "fluidsim/ISimCommandList.h"
#include "renderers/splatting/splatting-renderer.h"

class ParticleListBuilder {
	friend class ParticleManager;

protected:
	std::vector<AddParticle> particles;
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
	std::shared_ptr<IFluidSimulation> sim;

	[[nodiscard]] ISimCommandList *CreateCommandListFromBuilder(
		const ParticleListBuilder &builder
	) const;

public:
	explicit ParticleManager(const std::shared_ptr<IFluidSimulation> &sim);
	~ParticleManager() = default;

	static ParticleListBuilder CreateParticleList();
	void AddParticles(const ParticleListBuilder &builder, const std::shared_ptr<gelly::renderer::splatting::AbsorptionModifier>& absorptionModifier) const;
	void ClearParticles() const;
};

#endif	// PARTICLES_H
